# Unit test for ImGUI.

exec(open('./import_common.py').read())

scene = engine.Scene()

clearCamera = engine.Camera()
clearCamera.setName('clear')
scene.add( clearCamera )

gui = engine.Camera()
gui.setName('ImGui')
scene.add( gui )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
clearCamera.setRenderContext( context )
gui.setRenderContext( context )

renderPass = engine.IRenderPass.create()
clearCamera.setRenderPass( renderPass )

guiPass = engine.IRenderPass.create()
gui.setRenderPass( guiPass )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)
renderTarget.setClear( True )

renderPass.addRenderTarget( renderTarget )
guiPass.addRenderTarget( renderTarget ) 

clearScreen = engine.ClearScreen()
clearCamera.addChild( clearScreen )

#imgui = engine.ImGUI()
#gui.addChild( imgui )

print( 'Exporting ' + exportPath + 'imgui.ieb' )
scene.save(exportPath + 'imgui.ieb')

print( 'Success!' )
