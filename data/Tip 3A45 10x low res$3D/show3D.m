function show3D(fname)
%Read file.
[x, y, z, r, g, b, mask] = xyzmread(fname);

%Display 3D
figure(1);
z = z(50:115, 250:350);  %eye region - cheat for now.
surf( z, 'FaceColor', 'interp',...
        'EdgeColor', 'none',...
        'FaceLighting', 'phong');
camlight right
colormap(summer)
set(gca, 'DataAspectRatio', [1, 1, 1],... %Scaling axes appropriately
    'lineWidth', 1);
axis off  % Remove axis

%Set viewing parameters
%view(180, 90)
%view(-84, 22)
view(-90, 15)

%Get and draw normal.
%Define the region of interest (cheat for now)
roi = [34 44; ...
       38 48];
roicenter = [mean(roi(:, 1)) mean(roi(:, 2))];  %y x: rows, cols
%Compute the normal and scale it.
hatNorm = computeHatNormal(z(roi(1,1):roi(2, 1), roi(1,2):roi(2,2)));
hatNorm = hatNorm * 30;
%Plot the normal.
figure(1)
hold on
plot3([roicenter(2) (roicenter(2) + hatNorm(1))], ...
     [roicenter(1) (roicenter(1) + hatNorm(2))], ...
     [z(round(roicenter(1)), round(roicenter(2))) ...
     (z(round(roicenter(1)), round(roicenter(2))) + hatNorm(3))], 'r-*');

end