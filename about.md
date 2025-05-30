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

# Note About Colors with Blending
**Doing additive blending on a transparent background is physically impossible.** 
As such, the mod has an option that approximates the opacity for objects with blending using a makeshift method. The mod will ask you if you want to enable it each time you export.  
***The option appears to be better for some images, but worse for others. It just depends.***
  
*The method with which it does this, which is taking the highest of the R, G, and B values and setting the opacity to that, is pretty makeshift.  
If anyone has a better idea, feel free to suggest it or make a [Pull Request](https://github.com/rgc-exists/ObjectsToImage).*

# Platform Support
For now, this mod only supports Windows, as I doubt Android and/or iOS would work without major changes.  
UPDATE: The mod has been 100% confirmed to NOT work on MacOS as is.
