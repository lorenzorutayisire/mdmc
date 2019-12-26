# mdmc

mdmc is a software able to convert your beautiful 3D model, into a Minecraft schematic, keeping the most details it can.

![mdmc_preview](https://i.ibb.co/FKqdGnQ/mdmc-skull-2.png)

## Version 0.1a

mdmc will:
* Load, from the specified Minecraft version, _only the full, equally faced, blocks_ (so dirt, wood, bedrock...).
* Load the model specified from command line.
* Show a preview of the loaded model (useful for debug).
* Voxelize it.
* Show a preview of the voxelized model.
* Show a preview of the Minecraft mapped model.

## Version 0.2a (WIP)
* Download Minecraft assets from https://mcassets.cloud.
* Load the entire Minecraft blocks assets (excluding block entities).
* Use an higher resolution, which means more voxels will be used per block in order to decide which Minecraft model associate (similar to multisampling).
* Export to Sponge-Specification NBT schematic.
* Show some awesome details with imgui.

## Demo

Say here's the model you found on some awesome game and you'd like to have it on Minecraft.

![mdmc_from_1](https://i.ibb.co/0yMxRmz/MDMC-from.png)

What you'll get, with a resolution of 74 blocks high, is something like this:

![mdmc_to_1](https://i.ibb.co/xsTYdYx/MDMC-4.png)

Unfortunately you can't export it yet. My plans are to implement the newest [Sponge Schematic format](https://github.com/SpongePowered/Schematic-Specification/blob/master/versions/schematic-2.md).
