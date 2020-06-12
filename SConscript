from building import *
Import('rtconfig')

src   = []
cwd   = GetCurrentDir()

# add pmsxx src files.
if GetDepend('PKG_USING_PMSXX'):
    src += Glob('src/pmsxx.c')
    src += Glob('src/sensor_plantower_pmsxx.c')

if GetDepend('PKG_USING_PMSXX_SAMPLE'):
    src += Glob('examples/pmsxx_sample.c')
    src += Glob('examples/sensor_pmsxx_sample.c')

# add pmsxx include path.
path  = [cwd + '/inc']

# add src and include to group.
group = DefineGroup('pmsxx', src, depend = ['PKG_USING_PMSXX'], CPPPATH = path)

Return('group')
