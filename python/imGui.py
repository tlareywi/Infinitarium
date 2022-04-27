# Unit test for ImGUI.

exec(open('./import_common.py').read())

scene = engine.Scene()
scene.setName('ImGui Scene')

#clearCamera = engine.Camera()
#clearCamera.setName('Clear Camera')
#scene.add( clearCamera )

gui = engine.Camera()
gui.setName('ImGui Camera')
scene.add( gui )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
#clearCamera.setRenderContext( context )
gui.setRenderContext( context )

#renderPass = engine.IRenderPass.create()
#clearCamera.setRenderPass( renderPass )

guiPass = engine.IRenderPass.create()
gui.setRenderPass( guiPass )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)

#renderPass.addRenderTarget( renderTarget, engine.LoadOp.Clear )
guiPass.addRenderTarget( renderTarget, engine.LoadOp.Clear ) 

#clearScreen = engine.ClearScreen()
#clearScreen.setName('Clear Renderable')
#clearCamera.addChild( clearScreen )

imgui = engine.ImGUI()
imgui.setName('GUI Renderable')
gui.addChild( imgui )

os.chdir(homeDir)

print( 'Exporting ' + exportPath + 'imgui.ieb' )
scene.save(exportPath + 'imgui.ieb')

print( 'Success!' )
