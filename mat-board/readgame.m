%READGAME(F)
%
% read game from file
%
function game = readgame(f)

if ischar(f)
  f = fopen(f, 'r');
end

c_player = nextch(f);
game.player = ch2player(c_player);

game.halfstep = fscanf(f, '%d', 1);

c_winner = nextch(f);
if strcmpi(c_winner, 'U')
  game.winner = 0;
elseif strcmpi(c_winner, 'D')
  game.winner = 3;
else
  game.winner = ch2player(c_winner);
end

game.score = [0 0];
game.time = [0 0];

for p = 1:2
  game.score(p) = fscanf(f, '%d', 1);
  game.time(p) = fscanf(f, '%f', 1);
end

game.matrix = readboard(f);

fclose(f);

%CH2PLAYER(CH)
function player = ch2player(ch)
  if strcmpi(ch, 'A') 
    player = 1;
  elseif strcmpi(ch, 'B')
    player = 2;
  else
    error('Invalid player char "%c"', ch);
  end

%READBOARD(F)
function B = readboard(f)
  B = char(ones(8)*'-');
  for i = 1:8
    for j = 1:8
      B(i,j) = nextch(f);
    end
  end
  
%NEXTCH(F)
function c = nextch(f)
  c = 0;
  allowed = '-12ABUD*';
  spaces = char(1:32);
  while true
    c = fgets(f,1);
    if ismember(c, allowed)
      break;
    elseif ~ismember(c, spaces)
      error('Invalid character "%c" in file', c);
    end
  end
    