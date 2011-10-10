%PLOTBOARD(B, MOVE)
%
% B:    matrix 8x8
% MOVE: [] or movement description
%       formatted [i0, j0; i1, j1] 
%
% plot chess board with 
%   - pieces ('A', 'B')
%   - star/digit marks ('*', '0', '1')
%   - empty cells ('-')
%
function plotboard(B, move)

if ~exist('move', 'var')
  move = [];
end

hold on;
axis manual;
axis ij;
axis square;
axis off;
axis([0 8 0 8]);

bgcolors = [1   1   1     % white
            0.5 0.5 0.5]; % gray
for i = 1:8
  for j = 1:8
    c = mod(i+j,2)+1;
    fill([j-1 j j j-1], [i-1 i-1 i i], bgcolors(c,:));
    
    switch B(i,j)
      case 'A'
        plotcircle(i, j, 'r');
      case 'B'
        plotcircle(i, j, 'b');
      case {'*', '1', '2'}
        plotchar(i, j, B(i,j));
    end
  end
end
    
plotarrow(arrowcoord(move(:,2)), arrowcoord(move(:,1)));


%PLOTCIRCLE(I,J,COLOR)
function plotcircle(i, j, color)
  n = 50;
  d = 0.8;

  x0 = j-0.5;
  y0 = i-0.5;
  r = d/2;
  alpha = 2*pi/n * [0:n-1];
  X = r*cos(alpha) + x0;
  Y = r*sin(alpha) + y0;
  
  fill(X, Y, color);
  
%PLOTCHAR(I,J,CH)
function plotchar(i, j, ch)
  line_width = 4;
  x0 = j-0.5;
  y0 = i-0.5;
  switch ch
    case '*'
      r = 0.3;
      plot([x0-r x0+r], [y0 y0], 'k', 'LineWidth', line_width);
      plot([x0 x0], [y0-r y0+r], 'k', 'LineWidth', line_width);
      r = r*sqrt(2)/2;
      plot([x0-r x0+r], [y0-r y0+r], 'k', 'LineWidth', line_width);
      plot([x0-r x0+r], [y0+r y0-r], 'k', 'LineWidth', line_width);
    
    case '1'
      r = 0.3;
      plot([x0 x0], [y0-r y0+r], 'k', 'LineWidth', line_width);
      plot([x0-0.1 x0], [y0-r y0-r],  'k', 'LineWidth', line_width);
      plot([x0-0.1 x0+0.1], [y0+r y0+r],  'k', 'LineWidth', line_width);
    
    case '2'
      r = 0.3;
      rw = r*0.7;
      plot([x0-rw x0+rw x0+rw x0-rw x0-rw x0+rw], [y0-r y0-r y0 y0 y0+r y0+r], ...
        'k', 'LineWidth', line_width);
 
  end
  
%PLOTARROW(X,Y)
function plotarrow(x, y)
  line_width = 2;
  color = [0 0.8 0];
  plot(x, y, 'Color', color, 'LineWidth', line_width);
  v = [x(2)-x(1) y(2)-y(1)]';
  v = v / norm(v) * 0.2;
  alpha = 30 * pi/180;
  R = [cos(alpha) -sin(alpha); sin(alpha) cos(alpha)];
  v1 = R * v;
  v2 = R' * v;
  plot([x(2)-v1(1) x(2)], [y(2)-v1(2) y(2)], 'Color', color, 'LineWidth', line_width);
  plot([x(2)-v2(1) x(2)], [y(2)-v2(2) y(2)], 'Color', color, 'LineWidth', line_width);
  

%ARROWCOORD(X)
function X = arrowcoord(X)
  r = 0.2;
  if X(1) < X(2)
    x0 = X(1)-r;
    x1 = X(2)-1+r;
  elseif X(1) > X(2)
    x0 = X(1)-1+r;
    x1 = X(2)-r;
  else
    x0 = X(1)-0.5;
    x1 = X(2)-0.5;
  end
  X = [x0 x1];


 
