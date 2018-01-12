# sun-system
An OpenGL program which simulates the sun-system.

Als Projekt habe ich die Nachmodellierung unseres Sonnensystems gewählt, da in diesem Beleuchtung, Texturierung und abhängige Rotationen sehr gut umsetzbar ist.

Folgende Anforderungen sollen erfüllt werden:
- Leuchtende Sonne und beleuchtete Planeten
- maßstabsgetreue Skalierung der Planeten
	- Durchmesser
	- Geschwindigkeit um die Sonne 
- Planeten rotieren um die Sonne und um die eigene Achse
- Planeten erhalten entsprechende Texturen
- mit der Leertaste werden alle Rotationen gestoppt/fortgesetzt
- mit den hoch/runter Pfeiltasten kann die Rotationsgeschwindigkeit verschnellert/verlangsamt werden
- mit den rechts/links Pfeiltasten kann zwischen den verschiedenen Kameraperspektiven umgeschalten werden
	- Sicht auf das komplette Sonnensystem
	- Sicht auf die rotierenden Planeten

Dabei ist die Winkelgeschwindigkeit der einzelnen Planeten zu vernachlässigen, da diese sich mit der Zeit ändert und dies den Rahmen sprengen würde. Deswegen wird lediglich eine Rotation gegen den Uhrzeigersinn realisiert. Weiterhin ist der Abstand zur Sonne ebenfalls nicht maßstabsgetreu, da sonst das Koordinatensystem zu groß würde. Aus diesem Grund ist der Abstand der Planeten untereinander gleich.

Die Texturen der Planeten habe ich unter folgender URL gefunden: http://www.solarsystemscope.com/textures/
Die Informationen zu den Planeten habe ich unter folgender URL gefunden: https://astrokramkiste.de/planeten-tabelle
http://ftextures.com/textures/solar-panel-module.jpg


http://hhh316.deviantart.com/art/Seamless-metal-texture-182943398
http://henker144.deviantart.com/art/Gold-2-178145570