# ObjectsToImage
Convert selected GD objects to an image.

# How to Use
The mod adds a new button to the "edit" tab in the editor. Press this button and select a path you want to save the file to.  
  
*Note: Sometimes when you are in fullscreen the file explorer popup will appear behind the game window. You will need to open the taskbar by pressing the windows key, then click the icon of the game and select the file explorer window.*

# Setting the Default Path
When you save an image to a path, the mod will use the directory you chose as the default when the file explorer opens from then on, UNTIL you close the game.  
If you want to set the default directory to something other than the AppData directory WITHOUT navigating to it the first time, there is a "default directory" setting available in the mod's geode settings.

# Tips
- If you want to capture the state of an object AFTER it moves or rotates in-game, you can play the level in editor mode andd PAUSE the playmode instead of stopping it, then select the desired objects and save while still not paused.
  
- If you wish to capture an object with the colors a certain way, but the colors don't align the way you want when the camera is in range of the objects, you can press F1 in another part of the level to "lock the colors". You can also press F2 later to unlock them.

# Platform Support
For now, this mod only supports Windows, as I doubt Android and/or iOS would work without major changes.  
UPDATE: The mod has been 100% confirmed to NOT work on MacOS as is.

## Special Thanks
I am still somewhat inexperienced with GD mod development, so I'd like to thank some other mods whose code I took inspiration from.
- [Reference Image](https://github.com/matcool/geode-mods/blob/main/reference-image/main.cpp) - For showing me how to add a button to the editor.
- [Object Workshop](https://github.com/FireMario211/Object-Workshop/blob/66548f35fa3929e5168e1103596565ac4caedc5b/src/nodes/ObjectItem.cpp#L167-L184) - For showing a better method of creating object images than my original, much messier and horrible method of capturing all the objects manually. 
- HiImJasmine, Undefined06855, Firee Fleeym, Prevter, and others from the #dev-help channel in the Geode discord server. I could not have done this without the help of more experienced modders. :)