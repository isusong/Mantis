function hatNorm = computeHatNormal(z)
%z should be region of interest data for the eye peak.

%Do interpolation
tx = 1:0.1:size(z, 2);
ty = 1:0.1:size(z, 1);
[x, y] = meshgrid(tx, ty);  %Make the appropriate 2D x and y grids.
zinterp = interp2(z, x, y, 'cubic'); %cubic interpolation

%Get normals
hatNorm = zeros(3, 1);
[Nx, Ny, Nz] = surfnorm(zinterp);
%Average normals.
hatNorm(1) = mean(mean(Nx));
hatNorm(2) = mean(mean(Ny));
hatNorm(3) = mean(mean(Nz));

%Plot this interpolated region.
figure(2)
surf(x, y, zinterp, 'FaceColor', 'interp',...
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
end