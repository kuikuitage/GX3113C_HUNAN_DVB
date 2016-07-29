
# -*- coding: cp936 -*-

##
# Module/script example of the xlrd API for extracting information
# about named references, named constants, etc.
#
# <p>Copyright ?2006 Stephen John Machin, Lingfo Pty Ltd</p>
# <p>This module is part of the xlrd package, which is released under a BSD-style licence.</p>
##

import xlrd
import sys, os
import glob
import types
import codecs

xml_file = None
bk = None

def create_xml():

    global xml_file
    global bk

    cwd = os.getcwd()
    xml_name = cwd + "/" + "i18n.xml"
    xls_name = cwd + "/" + "i18n.xls"
    print "xml = %s" % (xml_name)

    #xml_file = open (xml_name, "w")
    xml_file = codecs.open (xml_name, "w", "gb2312")
    bk = xlrd.open_workbook(xls_name)

def create_i18n_xml():

    global xml_file
    global bk

    sh = bk.sheet_by_index(0)
    nrows = sh.nrows
    ncols = sh.ncols
    print "rows = %d; cols = %d" % (nrows, ncols)

    print >> xml_file, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>"
    print >> xml_file, "<i18n>"
	
    for i in range(1,ncols):
        row_data = sh.row_values(0)
        print >> xml_file, "    <language name=\"%s\">" % (row_data[i])
        for j in range(1,nrows):
            row_data = sh.row_values(j)
            print >> xml_file, "        <tran id=\"%s\"                >%s</tran>" % (row_data[0], row_data[i])
	    #data = "        <tran id=\"" + row_data[0] + "\"" + "             >" + row_data[i] + "</tran>\n"
	    #xml_file.write(data)


        print >> xml_file, "    </language>"

    print >> xml_file, "</i18n>"

    xml_file.close()

if __name__ == "__main__":
    create_xml()
    create_i18n_xml()
