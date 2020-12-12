# Creates a minimal scene for application startup state.

exec(open('./import_common.py').read())

scene = engine.Scene()
scene.setName('Empty Scene')

camera = engine.Camera()
camera.setName('Clear Camera')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
camera.setRenderContext( context )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)
renderPass.addRenderTarget( renderTarget, engine.LoadOp.Clear )

clearScreen = engine.ClearScreen()
clearScreen.setName('Clear Renderable')
camera.addChild( clearScreen )

print( 'Exporting ' + exportPath + 'empty.ieb' )
scene.save(exportPath + 'empty.ieb')

print( 'Success!' )
