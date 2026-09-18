# function fish_prompt
#     echo $PWD (set_color cyan) '> '
# end

function fish_prompt
    string join '' -- (set_color normal) (prompt_pwd) (set_color cyan) ' < '
end
set fish_greeting ""
if status is-interactive


fish_add_path ~/.local/bin
fish_add_path ~/bin


alias ka='killall'
alias lv='lavat -c cyan -k red'
alias f='fastfetch'
alias q="printf '\033[2J\033[3J\033[1;1H'"
alias n='neofetch'
alias x='startx'
alias cma='cmatrix -rmu 6'
alias ls='eza'
end
