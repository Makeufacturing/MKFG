# Laser Cutter (OMTech 55W, 16x24")

## Initial Setup

* Carefully uncrate.
  * Ensure nothing appears damaged
  * Ensure all main components are included
* Position machine and level/secure feet.
* Remove any packing elements (foam around laser tube, zip-ties around keys, covers over mirrors, etc.). Gently move the lens/head element to make sure the linear slides are well lubricated and operational.
* Ensure ground is well connected! If things go wrong and ground isn't connected, people can die.
  * A ground wire is included, but adding a secondary ground is not actually advised (ground loops and perhaps confusing the GFCI inline with the outlet).
  * Instead, make sure you are using a well grounded, 3-prong outlet and plug. 
  * Use a multimeter to ensure that the ground connection is both (1) well connected to the entire frame of the machine, and (2) well connected to the outlet itself.
* Note: **Never fully trust the safety protections built into the machine!** **Wear laser safety glasses** (designed for CO2), etc. and be skeptical of anything that should _automatically_ keep you safe!
* Set up 5 gallon bucket for distilled water. Start with water only, and then consider the "secret recipe" of algae inhibitor and a few drops of soap as advised in many online forums. Connect all tubing as directed.
* Install a higher flow rate exhaust fan and connect to an outlet the fully leaves the building. This is important for both fume extraction and clearing dust/smoke from the cutter to preserve the lens and mirrors.
* Prime the laser tube with coolant/water by filling the bucket until the pump is submerged and then plugging the pump in (directly to the wall for now). Ensure there are no leaks and no bubbles in the tube. If bubble remain in the laser tube, stop and start the pump a couple of time to flush them out. Once primed and ready, plug the water pump into the laser cutter's water pump electrical outlet.

## Initial Power On/Test

* Wear safety glasses. Ensure all doors closed on the cutter and **all cutter power switches turned OFF**. Ensure there is nothing in the cutting area that could restrict the motion of the laser head.
* Plug in the machine.
* Make sure the E-Stop (twisted CW until it pop out) and Laser key switch (rated CW) are in the on position.
* Turn on the Water Pump switch (it will not actually turn on until the next step).
* Turn on the Control switch. The machine should power up and home. Ensure the water pump is functional at this time.
* Place a piece of test material (cardboard) under the laser head and appropriately focused.
* Turn on the Laser switch.
* **Laser is about to fire for first time.** Press the "Pulse" button once. You should see a very brief flash and a tiny mark on the test material. Hooray :) 
* Now turn off the laser switch, place a piece of tape over the opening to the final mirror/laser head and pulse at the extremes of the machine's X/Y. Ideally the hole should be in the center and identical for all corners.
* Ensure the Max Power setting in Ruida is set to ~50%. Laser cutter docs specify that staying below 50% greatly extends the life of the tube, so unless there's a specific need (i.e. cutting very thick material), keeping the max power down seems wise. Multiple passes may be a better option than increasing the power for most thick cuts.

<p style="page-break-after: always;">&nbsp;</p>

## Laser Power ON Sequence

1. Wear **safety glasses** ( :dark_sunglasses: ) designed for a CO2 laser (10.6&mu;m). 

2. **Do internal mirrors need wiped down?** Dust on mirrors can reduce the overall output power of the cutter significantly (and potentially crack/pit the mirrors if too much accumulates).

3. Ensure **all doors are closed** on the laser cutter and there is **nothing in the cutting area** that could restrict the motion of the laser head.
4. Turn **ON wall power switch**. The machine should power up and home. 
5. Ensure the **water pump** is functional and there is adequate/clean water in the bucket.
6. Ensure the exhaust fan is functional.
7. Turn **Laser Key** to **ON/CW** and leave key in machine. Leave the Laser Switch on the side of the cutter in the OFF position until needed. 
8. When material is placed and the desired file has been loaded, turn **ON** the Laser Switch before cutting.

* ⚠️ Always turn the **Laser Switch OFF** any time your body may enter the laser beam's path, before performing maintenance, cleaning a lens, etc. to reduce the chance of accidents. For more significant adjustments (especially regarding the laser tube itself), fully unplug the machine and wait 30 minutes prior to handling.
* ⚠️ **NEVER RUN THE MACHINE UNATTENDED.** Cutting flammable materials with a laser can easily cause flare-ups and fires. 🔥 Immediately stop any job (e.x. via pressing the E-Stop switch) if a fire is observed or the machine otherwise seems unsafe.



## Laser Power OFF Sequence

1. Turn **Laser Switch** and **Laser Key** both to **OFF/CCW**. 
2. **Remove keys** from machine and put away.
3. Turn **OFF wall power switch**.
4. **Return safety glasses** ( :dark_sunglasses: ) and **clean up** material.

<p style="page-break-after: always;">&nbsp;</p>

## Laser Materials / Settings

All settings for OMTech **55W CO2 Laser**. Max cutting area is **590x400mm**. 

All settings specified as **[Speed(mm/sec):Power%/PowerMin%:(LineHeight/mm)]**.

See the [ULS Materials Library](https://www.ulsinc.com/material/materials-library) for lots of good info on what materials can be cut and how.

### Cutting

Assuming 3.0" focal length (FL) lens unless noted. **If using 1.5" lens, increase speed by ~10-15% (seems to be slightly more efficient, but not as good for deeper cuts).**
  :warning: If it's been a couple weeks since you last checked, **make sure the mirrors are clean!** Dirty mirrors greatly reduce the overall cutting power.

* **Cardboard** (common): [20:20%] or... [40:40%] for faster/cleaner cut!
* **Cardstock** (120lb, 325gsm): [80:20%]
* **Veneer (Maple)**: [35: 26%]
* **1/8" Birch Ply**: [14:32%] --> [10:45%]
* **5mm Birch Ply**: [7:42%] --> reliable with cleaned mirrors! (2 passes at [8:40%] is reliable back when mirrors were dirty).
* **1/4" Birch Ply**: [6:50%] --> reliable with cleaned mirrors!
* **1/2" Birch Ply** (3" FL lens, actual material thickness 0.475"): [5:40%] focus into material ~0.1"
* **Painter's Tape** (green/frog/blue): [70:12%]
  * For least cut through painters tape over another surface as a stencil: [70:11%] or [40:10%] or [18:9.7%]
* **1/16" Felt** (adhesive back/tape): [12:26%]
* **0.015"-0.035" Silicone Mat/Sheet**: [24:35%] Placed card stock under to support silicone. Color may affect cut, so test to be sure.
* **0.001" HDPE film** (standard thin grocery/take-out bag): [100:11%] Taped around the edges on top of card stock to keep flat. Some melting occurs (holes are slightly widened), but overall a clean cut is surprisingly achievable. Works with both beige and white bags, but the white seemed more consistent.
* **Marine Canvas** (Polyester, with PU backing, 600 Denier): [14:35%]
* Cotton
* **1/16" Poly Propylene**: [10:38%]
* **Clear Acrylic** (1/16", w/ paper coating): [12:34%]
* **Thin(!) Polycarbonate** (0.015in thick): [14:20%]
  * Barely cut through clear film on PC sheet: 
    * Longer cuts [72:12%] (5mm+ lines/curves)
    * Shorter cuts [100:10] (tiny icons)
  * If stenciling, remove protective coating from top and apply blue painter's tape. Then use the "least cut through" method [~18:9.7%/9.1%] to nicely cut features.
* **EPDM Rubber**
  * 1/64" thick (black) w/ adhesive: [10:15%]
* **Neoprene Rubber**
  * 1/64" thick (**White**) w/ adhesive, covered with blue painter's tape over the top: [10:18.5%/16.5%].
  * 1/64" thick (**Black**) w/ adhesive, covered with blue painter's tape over the top: [10:17.5%/16.5%].
* **Latex Rubber** (Heavy / Medium): [70:25%] Cuts easily, but is sticky around edges.

### Engraving

* **Deep wood fiber etch** (cardstock/paper/tape): [70:12%:0.1mm]
* **Cardboard graphics etch**: [400:24%-21%:0.1mm]
* **Painter's Tape Etch-through**: [140:12%:0.1mm] + [90:12%] line after
* **Aluminum (6061) w/ CerMark LMM-6000**: 
  * Sand w/ 400 grit+, wipe dry, then wipe with 70%+ isopropyl.
  * Apply thin, even coat of marking paint, and let dry 25 min.
  * [50:70%;0.1mm] + [40:50%] line after



## Lens Cleaning/Change

* Wear **safety glasses** ( :dark_sunglasses: ) designed for a CO2 laser (10.6&mu;m). 
* Ensure **Laser Power OFF** (both **Switch** and **Key**).
* Move head to center of work area and lower bed to give space.
* Gently disconnect the air assist tube from the lower end of the head.
* Place a cloth under the head to catch anything as you work (lenses are expensive; be careful pls!).
* The lowest section of the head (only containing the air assist connector) can unscrew. You may need to slightly rotate the air assist or adjust the red laser guide to unscrew it.
* If there is only **minor** dust:
  * use a bulb air blower to clear any debris.
* If there is **significant/visible debris** on the lens:
  * Place a pre-moistened alcohol wipe on the soft surface below the head.
  * Very carefully unscrew and tip out the lens onto the wipe (both the lens and the white compression ring will fall out)!
  * Gently wipe the lens until it is clean.
  * Using the wipe (so as to not add any oils from your fingers), place the lens back into the head, with the more curved side of the lens facing up.
  * Place the compression ring on top of the lens, and screw in the retaining collar. 
* Carefully screw the lens section back onto the head (it should be "finger tight"); do not over-tighten.
* Re-connect the air assist tube to the head and make sure the red laser dot is approximately correct (it should be directed from the back of the machine).
* Return the head to home and ensure everything mechanically clears the rails.
* Return the bed to the appropriate focal distance for the lens, and re-align the red laser to match the laser's output.

