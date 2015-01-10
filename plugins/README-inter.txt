Imported #410806 from gst-plugins-bad

Run:
sed -i -e's/GstInter/GswInter/g' \
   -e's/GST_TYPE_INTER/GSW_TYPE_INTER/g' \
   -e's/gst_inter_/gsw_inter_/g' *inter*.[ch]
to create GObjects with non-conflicting names

Run:
sed -i -e's/ \"inter/ \"gswinter' gstinter.c

to register elements as 'gswintervideosrc' etc instead of plain 'intervideosrc'
