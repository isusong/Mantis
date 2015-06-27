/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * This computer software was prepared by The Ames 
 * Laboratory, hereinafter the Contractor, under 
 * Interagency Agreement number 2009-DN-R-119 between 
 * the National Institute of Justice (NIJ) and the 
 * Department of Energy (DOE). All rights in the computer 
 * software are reserved by NIJ/DOE on behalf of the 
 * United States Government and the Contractor as provided 
 * in its Contract, DE-AC02-07CH11358.  You are authorized 
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.  
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
 * OF THIS SOFTWARE.  
 *
 * This notice including this sentence 
 * must appear on any copies of this computer software.
 *
 * Author: Laura Ekstrand (ldmil@iastate.edu)
 */

#include "RangeImageViewer.h"
#include <QIcon>
#include <QColorDialog>
#include "GuiSettings.h"

//=======================================================================
//=======================================================================
RangeImageViewer::RangeImageViewer(PRangeImage rangeImage, bool useSplitMode, bool useViewTools, int flatDimension, int w, int h, QWidget* parent):
    QWidget(parent)
{
    _useViewTools = useViewTools;

    QGLFormat format;
    format.setDoubleBuffer(true);
    format.setDepth(true);
    format.setStencil(true);
    format.setAlpha(false);
    format.setSampleBuffers(true);
    format.setSamples(4);

    _width = w;
    _height = h;
    _mainGrid = new QGridLayout(this);
    _graphics = new GraphicsWidget2(format, useSplitMode, GraphicsWidget2::None, this);
    _renderer = PGenericModel(new RangeImageRenderer(rangeImage, this));

    // set searchbox color
    QColor c = GuiSettings::colorSearchBoxLeft();
    _renderer->setSearchBoxColor(c.redF(), c.greenF(), c.blueF(), c.alphaF()); // blue search box for right side

    if (useSplitMode)
    {
        _renderer2 = PGenericModel(new RangeImageRenderer(rangeImage, this));
        QColor c = GuiSettings::colorSearchBoxRight();
        _renderer2->setSearchBoxColor(c.redF(), c.greenF(), c.blueF(), c.alphaF()); // blue search box for right side
        _cmpSlider = new QSlider(Qt::Horizontal, this);
    }
    else
    {
        _cmpSlider = NULL;
    }



    _viewTools = NULL;
    _actionGroup = NULL;
    _shaded = NULL;
    _wireframe = NULL;
    _textured = NULL;
    _heightMapped = NULL;
    _signalMapper = NULL;
    _localCsys = NULL;
    _worldCsys = NULL;
    _background = NULL;

    if (_useViewTools)
    {
        _viewTools = new QToolBar(this);
        _actionGroup = new QActionGroup(_viewTools);
        _shaded = new QAction(this);
        _wireframe = new QAction(this);
        _textured = new QAction(this);
        _heightMapped = new QAction(this);
        _signalMapper = new QSignalMapper(this);
        _localCsys = new QAction(this);
        _worldCsys = new QAction(this);
        _background = new QAction(this);
    }

	//Adjust for flat dimension.
    setFlatDimension(flatDimension);

	initActions();
	makeConnections();
	assemble();

	//Some start up defaults.
    _graphics->setDrawCS(false);
    if (getRenderer(0)) getRenderer(0)->setDrawCS(false);
    if (getRenderer(0)) getRenderer(0)->setDrawMode(2); //start in textured.
    if (getRenderer(1)) getRenderer(1)->setDrawCS(false);
    if (getRenderer(1)) getRenderer(1)->setDrawMode(2); //start in textured.
    _graphics->updateGL();
}

//=======================================================================
//=======================================================================
RangeImageViewer::~RangeImageViewer()
{
	//Everything is parented.
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setModel(PRangeImage ri, int viewer, bool isTip)
{
    PGenericModel mdl;
    if (viewer == 1)
    {
        mdl = _renderer2;
    }
    else
    {
        mdl = _renderer;
    }

    if (!mdl)
    {
        RangeImageRenderer *renderer = new RangeImageRenderer(ri, this);
        mdl.reset(renderer);

        if (viewer == 1)
        {
            QColor c = GuiSettings::colorSearchBoxRight();
            renderer->setSearchBoxColor(c.redF(), c.greenF(), c.blueF(), c.alphaF()); // blue search box for right side
        }
    }
    else
    {
        ((RangeImageRenderer *)mdl.get())->setModel(ri);
    }

    ((RangeImageRenderer *)mdl.get())->setIsTip(isTip);

    _graphics->setModel(mdl, viewer); // need to refresh values, like bounding box
    _graphics->updateSplitStatsSelection();

    if (isTip)
    {
        _graphics->setYaw(45, viewer); // tip defaults to 45
    }

    _graphics->updateGL();
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setWindowSelected(bool sel)
{
    if (!_graphics) return;

    _graphics->setWindowSelected(sel);
}

//=======================================================================
//=======================================================================
bool RangeImageViewer::getIsWindowSelected()
{
    if (!_graphics) return false;

    return _graphics->isWindowSelected();
}

//=======================================================================
//=======================================================================
RangeImageRenderer* RangeImageViewer::getRenderer(int num)
{
    if (num == 0)
    {
        return dynamic_cast<RangeImageRenderer *>(_renderer.get());
    }

    return dynamic_cast<RangeImageRenderer *>(_renderer2.get());
}

//=======================================================================
//=======================================================================
void RangeImageViewer::initActions()
{
    if (!_useViewTools) return;

	//Set up the actions.
	//Shaded button.
    _shaded->setCheckable(true);
    _shaded->setActionGroup(_actionGroup);
    _shaded->setIcon(QIcon(":/shading_modes/Icons/smooth.png"));
    _shaded->setToolTip(tr("Shaded view"));
    _signalMapper->setMapping(_shaded, 0);

	//Wireframe button
    _wireframe->setCheckable(true);
    _wireframe->setActionGroup(_actionGroup);
    _wireframe->setIcon(QIcon(":/shading_modes/Icons/wire.png"));
    _wireframe->setToolTip(tr("Wireframe view"));
    _signalMapper->setMapping(_wireframe, 1);
	
	//Textured button.
    _textured->setCheckable(true);
    _textured->setActionGroup(_actionGroup);
    _textured->setIcon(QIcon(":/shading_modes/Icons/textures.png"));
    _textured->setToolTip(tr("Textured view"));
    _signalMapper->setMapping(_textured, 2);
    _textured->setChecked(true);

	//Height map button.
    _heightMapped->setCheckable(true);
    _heightMapped->setActionGroup(_actionGroup);
    _heightMapped->setIcon(QIcon(":/shading_modes/Icons/color.png"));
    _heightMapped->setToolTip(tr("Color-mapped view"));
    _signalMapper->setMapping(_heightMapped, 3);

	//Local Csys button.
    _localCsys->setCheckable(true);
    _localCsys->setIcon(QIcon(":/controls/Icons/local-csys.png"));
    _localCsys->setToolTip(tr("Local coordinate system on/off"));
    _localCsys->setChecked(false);

	//World Csys button.
    _worldCsys->setCheckable(true);
    _worldCsys->setIcon(QIcon(":/controls/Icons/world-csys.png"));
    _worldCsys->setToolTip(tr("World coordinate system on/off"));
    _worldCsys->setChecked(false);

	//Background selection button
    _background->setIcon(QIcon(":/controls/Icons/color-wheel.png"));
    _background->setToolTip(tr("Change background color"));
}

//=======================================================================
//=======================================================================
void RangeImageViewer::makeConnections()
{
    if (_useViewTools)
    {
        connect(_shaded, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_wireframe, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_textured, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_heightMapped, SIGNAL(triggered()), _signalMapper, SLOT(map()));
        connect(_localCsys, SIGNAL(toggled(bool)), _renderer.get(), SLOT(setDrawCS(bool)));
        connect(_worldCsys, SIGNAL(toggled(bool)), _graphics, SLOT(setDrawCS(bool)));
        connect(_background, SIGNAL(triggered()), this, SLOT(selectBackgroundColor()));
        connect(_signalMapper, SIGNAL(mapped(int)), _renderer.get(), SLOT(setDrawMode(int)));
    }

    connect(_renderer.get(), SIGNAL(updateGL()), _graphics, SLOT(updateGL()));
    connect(_graphics, SIGNAL(doubleClicked(int, int)), this, SIGNAL(doubleClicked(int, int)));
    connect(_graphics, SIGNAL(onLButtonDown(int, int)), this, SLOT(slotLButtonDownGraphics(int, int)));

    if (_cmpSlider)
    {
        connect(_cmpSlider, SIGNAL(sliderMoved(int)), this, SLOT(slotCmpSliderChanged(int)));
        connect(_cmpSlider, SIGNAL(sliderReleased()), this, SLOT(slotCmpSliderReleased()));
        connect(_graphics, SIGNAL(onSplitterMoved(int,int)), this, SLOT(slotSplitterMoved(int,int)));
    }
}

//=======================================================================
//=======================================================================
void RangeImageViewer::assemble()
{
    _graphics->setModel(_renderer);
    _graphics->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);

    if (_useViewTools)
    {
        _viewTools->setOrientation(Qt::Vertical);
        _viewTools->addAction(_shaded);
        _viewTools->addAction(_wireframe);
        _viewTools->addAction(_textured);
        _viewTools->addAction(_heightMapped);
        _viewTools->addSeparator();
        _viewTools->addAction(_localCsys);
        _viewTools->addAction(_worldCsys);
        _viewTools->addSeparator();
        _viewTools->addAction(_background);
        _mainGrid->addWidget(_viewTools, 0, 0);
        _mainGrid->addWidget(_graphics, 0, 1);
    }
    else
    {
        setContentsMargins(0,0,0,0);
        _mainGrid->addWidget(_graphics, 0, 0);

        if (_cmpSlider)
        {
            QSpacerItem *vSpacer = new QSpacerItem(1, 20, QSizePolicy::Ignored, QSizePolicy::Ignored);
            _mainGrid->addItem(vSpacer, 1, 0);

            _mainGrid->addWidget(_cmpSlider, 2, 0);
        }

        _mainGrid->setSpacing(0);
        _mainGrid->setContentsMargins(0,0,0,0);
    }

    setLayout(_mainGrid);
    resize(_width, _height);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::selectBackgroundColor()
{
    QColor selection = QColorDialog::getColor("black", this, tr("Select Background Color"));
    if (selection.isValid()) _graphics->setBackgroundColor(selection);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setFlatDimension(int flatDimension)
{
	//Adjust for flat dimension.
	switch (flatDimension)
	{
		case 0:
            _graphics->setMouseMultipliers(QVector3D(6, 1, 1));
			break;
		case 1:
            _graphics->setMouseMultipliers(QVector3D(1, 6, 1));
			break;
		case 2:
            _graphics->setMouseMultipliers(QVector3D(1, 1, 6));
	}
}

//=======================================================================
//=======================================================================
void RangeImageViewer::rotate(float xAxis, float yAxis, float zAxis)
{
    getRenderer()->setTransformMatrix(QMatrix4x4());
    getRenderer()->rotate(zAxis, QVector3D(0, 0, 1)); //z roll
    getRenderer()->rotate(yAxis, QVector3D(0, 1, 0)); //y yaw
    getRenderer()->rotate(xAxis, QVector3D(1, 0, 0)); //x pitch
    _graphics->updateGL();
}

//=======================================================================
//=======================================================================
void RangeImageViewer::setSliderPos(float per)
{
    if (!_cmpSlider) return;
    float min = _cmpSlider->minimum();
    float max = _cmpSlider->maximum();

    int pos = (int)(min + (max - min)*per);
    _cmpSlider->setSliderPosition(pos);
    _graphics->setSplitterX(pos);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape)
    {
        setWindowSelected(false);
    }
}

//=======================================================================
//=======================================================================
void RangeImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;

    emit onLButtonDown(event->pos().x(), event->pos().y());
}

//=======================================================================
//=======================================================================
void RangeImageViewer::slotLButtonDownGraphics(int x, int y)
{
    emit onLButtonDown(x, y);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::slotCmpSliderChanged(int value)
{
    if (!_graphics) return;

    _graphics->setSplitterX(value);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::slotCmpSliderReleased()
{
    //emit updateStatsRT();
    emit onCmpSliderReleased();
}

//=======================================================================
//=======================================================================
void RangeImageViewer::slotSplitterMoved(int x,int y)
{
    Q_UNUSED(y);
    _cmpSlider->setSliderPosition(x);
}

//=======================================================================
//=======================================================================
void RangeImageViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (!_cmpSlider) return;
    if (!_graphics) return;


    QSize sz = _graphics->size();
    int loc = _graphics->getSplitterX();
    _cmpSlider->setMinimum(0);
    _cmpSlider->setMaximum(sz.width()-1);
    _cmpSlider->setSliderPosition(loc);
}
