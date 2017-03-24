
AntiGlyph


Words to be typed are read from a simple txt file,
and queue up from the bottom of the screen to the "firing range".
The "gun" fires ascii glyphs left to right at the word in the firing range.
When the fired glyph matches the glyph it hits,
the glyphs obliterate each-other, sending light and sparks upward.
Words may have punctuation, and always end with a space glyph.
The space glyph looks like a beam holding up the line, and
once it's obliterated, the queue moves up.
When the fired glyph doesn't match, it will stick in the firing range,
and glyphs fired at other stuck glyphs will always stick, even when they match.
Fire a backspace glyph to obliterate stuck glyphs,
Firing a backspace glyph at a queue glyph will have no effect;
the backspace glyph will simply fly off the screen.
Aside from backspace, mistyped glyphs can pile up, and if they pile up
all the way back to the gun, the gun will explode and the game will be lost.

The goal is to get through the entire txt file as quickly as you can,
and perhaps beat your high score for that file.
Some stats are displayed after you get through a file
  time total
  accuracy (% of glyphs that were correct)
  glyphs per minute
  words per minute (GPM*60/5)
Backspace is considered a correct glyph when it clears a mistake,
but otherwise backspace is itself counted as a mistake.

The interface is indifferent to the player's keyboard layout,
which makes it ideal for those trying out alternative layouts.
