TIME_IT = true


function default()
    print(PURPLE('Building bullet'))
    local b = builder()
    b.build_dir = 'build'
    b.compiler = 'g++'
    b.include_dirs = {
        'src',
    }
    b.src = table.merge(
        fs.scandir('src/*/*.cpp'),
        fs.scandir('src/*/*/*.cpp')
    )
    b.output = 'build/bullet.so'
    b:link(b:compile())
end

function clean()
    os.pexecute('rm -rf build')
end
