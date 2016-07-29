#!/bin/bash

rm -f widget.xml
NAME=`ls -tr *.xml`

echo '<?xml version="1.0" encoding="gb2312" standalone="no"?>'>./widget.xml

echo '<interface>'>>./widget.xml
echo '  <width>720</width>'>>./widget.xml
echo '  <height>576</height>'>>./widget.xml
echo '  <bpp>16</bpp>'>>./widget.xml
echo '  <osd_trans>#00FF00</osd_trans>'>>./widget.xml
echo '  <gui_trans>#FF00FF</gui_trans>'>>./widget.xml
echo '  <osd_alpha_global>#000000</osd_alpha_global>'>>./widget.xml
  

for i in $NAME ; do
	ID=${i%.xml}
	echo '  <widget class="window" style="default" name="'$ID'"/>'>>./widget.xml
done

echo "</interface>">>./widget.xml
