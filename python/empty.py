# Creates a minimal scene for application startup state.

exec(open('./import_common.py').read())

scene = engine.Scene()

camera = engine.Camera()
camera.setName('clear')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
camera.setRenderContext( context )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)
renderTarget.setClear( True )
renderPass.addRenderTarget( renderTarget )

clearScreen = engine.ClearScreen()
camera.addChild( clearScreen )

print( 'Exporting ' + exportPath + 'empty.ieb' )
scene.save(exportPath + 'empty.ieb')

print( 'Success!' )
