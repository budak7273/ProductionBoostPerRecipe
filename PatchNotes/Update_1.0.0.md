Library mod that allows mod developers to specify recipes that Somersloops can't be used on,
even if the machine itself supports Somersloops.



---

_If you enjoy my work, please consider donating to my [completely optional tip jar](https://ko-fi.com/robb4)._

Multiplayer compatible!

This expands the kinds of recipes that can be created, especially recipes with catalysts,
without worrying about Somersloops production bonus breaking their balance by giving you way too much catalyst back.

Without any other mods implementing somersloop-blocked recipes,
the only change this mod makes is
adding an icon to the machine recipe selection UI indicating when the machine does not support Somersloops -
so just the Packager in the base game.

## Example Screenshots

<!-- TODO point these to github raw URLs -->

When selecting a recipe in a machine that doesn't support Somersloops,
all recipes in the picker have a Stat to indicate this:
The Packager is the only base game production building that does not support Somersloops.

![Screenshot of the Packager recipe selection UI with the somersloop-disabled icon](https://raw.githubusercontent.com/budak7273/ProductionBoostPerRecipe/refs/heads/main/Assets/Modpage/DisabledInPickerByMachine.jpg)

---
> In the below screenshots, "Heat Sink" is used as an **_example only_** - this mod does not do anything to the heat sink recipe if you install it!
---

Recipes that don't support somersloops have a line added to their summary both in tooltips and the Codex:

_(Note: [TFIT](https://ficsit.app/mod/TFIT) mod is contributing additional tooltip elements in this screenshot.)_

![Screenshot of tooltip of a disabled recipe](https://raw.githubusercontent.com/budak7273/ProductionBoostPerRecipe/refs/heads/main/Assets/Modpage/DisabledInTooltipByRecipe.png)

_(Note: [TFIT](https://ficsit.app/mod/TFIT) and [Nog's Cheats](https://ficsit.app/mod/NogsCheats) mods are contributing additional visual elements in this screenshot.)_

![Screenshot of codex recipe listing](https://raw.githubusercontent.com/budak7273/ProductionBoostPerRecipe/refs/heads/main/Assets/Modpage/DisabledInCodexByRecipe.jpg)

If a Somersloop-restricted recipe is selected in a machine, Somersloops can still be added or removed,
but no Output Multiplier will be applied (1x) and visuals will be altered to indicate this:

![Screenshot of machine UI with a somersloop-disabled recipe selected](https://raw.githubusercontent.com/budak7273/ProductionBoostPerRecipe/refs/heads/main/Assets/Modpage/DisabledInMachineByRecipe.jpg)

The recipe picker also indicates which recipes are somersloop-disabled:

![Screenshot of recipe picker hovering a somersloop-disabled recipe](https://raw.githubusercontent.com/budak7273/ProductionBoostPerRecipe/refs/heads/main/Assets/Modpage/DisabledInPickerByRecipe.jpg)

## For Mod Developers

To make a recipe somersloop-incompatible:

1. Either create a blank mod in your editor with the same mod reference (`ProductionBoostPerRecipe`) as the real one,
   or [bring this mod into your project](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ImportingAnotherMod.html). (required for your mod to depend on this one)
1. Add the `ProductionBoostPerRecipe.DisableBoost`
   [Content Tag](https://docs.ficsit.app/satisfactory-modding/latest/Development/ModLoader/ContentTagRegistry.html) to the recipe you want to block somersloops on.
1. Add a [dependency on this mod in your `.uplugin`](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/ReleaseMod.html#_special_fields).
1. That's it, this mod takes care of the rest!

If you change your mind you can safely remove this mod without affecting your own, outside of the tag vanishing.

This should be compatible with any modded machine using the normal recipe production interface (`Widget_Manufacturing`)
as long as the machine doesn't override
`AFGBuildableFactory::SetPendingProductionBoost` or
`AFGBuildableManufacturer::SetRecipe`
without calling super (they are virtual functions).

## Side Notes

"Somerslooping" a recipe has multiple names in the code and community:

- [Production Amplification](https://satisfactory.wiki.gg/wiki/Production_amplifier) (in the MAM unlock)
- Output Multiplier (in machine UIs)
- Production Boost (in the code)
- Overslooping
- Slooping
