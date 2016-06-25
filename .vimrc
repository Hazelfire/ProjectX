set tabstop=4 softtabstop=4 shiftwidth=4

let &path.="Classes,"

set makeprg=make\ -C\ /home/sparka/Documents/ProjectX/proj.linux\ 

nnoremap <F4> :make!<cr>
nnoremap <F5> :!/home/sparka/Documents/ProjectX/proj.linux/bin/ProjectX<cr>
