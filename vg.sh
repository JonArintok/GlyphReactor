#!/bin/bash
valgrind --track-origins=yes --leak-check=full --suppressions=vg.supp ./AntiGlyph
