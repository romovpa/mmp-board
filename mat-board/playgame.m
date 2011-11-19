%PLAYGAME(TYPE,COMMANDS,MATRIX)
%
% run the game
%
% TYPE:      name of the game
% COMMANDS:  cell array of commands to run player
% MATRIX:    matrix filename
%
function outcome = playgame(type, commands, matrix_filename, vis)

if ~exist('vis', 'var')
  vis = 0;
end

MATRIX_FILE = 'matrix.txt';

copyfile(matrix_filename, MATRIX_FILE);

prev = [];
cur = readgame(MATRIX_FILE);
outcome = 0; % checkstep(prev, cur, type);
while outcome == 0
  player = cur.player;
  fullstep = floor((cur.halfstep+1)/2);
  id = sprintf('%c_%d', 'A'-1+player, fullstep);
  fprintf('%s: \n', id);
  
  % run executable
  tic;
  status = system(commands{player});
  time_actually = toc;
  if status ~= 0
    outcome = -1;
    descr = 'Program exited with non-zero code';
    break;
  end
  
  % save result
  copyfile(MATRIX_FILE, ['matrix_' id '.txt']);

  % update outcome
  prev = cur;
  cur = readgame(MATRIX_FILE);
  [outcome,descr,move] = checkstep(prev, cur, type);
  
  % print time
  time_change = prev.time(player) - cur.time(player);
  fprintf('Time elapsed: %f sec (%f actually)\n', time_change, time_actually);
  fprintf('Score: %d--%d\n', cur.score(1), cur.score(2));

  % visualization
  if vis
    plotboard(cur.matrix, move);
    if vis == 2
      print('-depsc2', sprintf('%c_%d_score_%d-%d.eps', 'A'-1+player, fullstep,...
        cur.score(1), cur.score(2)));
    else
      waitforbuttonpress;
    end
  end

  fprintf('\n');
end

if outcome < 0
  fprintf('Incorrect step: %s\n', descr);
  fprintf('Technical loss of player %c\n', 'A'-1+player);
else
  fprintf('Score: %d--%d\n', cur.score(1), cur.score(2));
  switch outcome
    case 1
      fprintf('Player A wins!\n');
    case 2
      fprintf('Player B wins!\n');
    case 3
      fprintf('Draw!\n');
  end
end
