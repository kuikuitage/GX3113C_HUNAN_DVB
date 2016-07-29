import sys, os
import codecs

sub_xml = None
subfile = None
subname = None

newname = None
newfile = None

def split_xml() :
	while True:
		file_name = raw_input('Enter File name:')
		cwd = os.getcwd()
		file_path = cwd + "/" + file_name	

		is_file = os.path.isfile(file_path)
		if is_file : break

	print file_path

	while True:
		file_format = raw_input('Enter File Format:')
		if file_format == 'utf-8' : break
		elif file_format == 'gb2312': break
		elif file_format == 'big5' : break

	print file_format

	xml_file = open(file_path, "r")

	newname = cwd + "/" + file_name + "_back"
	newfile = open(newname, "w")

	xmltitle = "<?xml version=\"1.0\" encoding=\"" + file_format + "\" standalone=\"yes\"?>\n"
	newfile.write(xmltitle);
	newfile.write('<i18n>\n');
	
	line = xml_file.readline()

	while line:
		line = xml_file.readline()
		head = line.find("<language name=")
		if(head >= 0) : 
			headline = "<?xml version=\"1.0\" encoding=\"" + file_format + "\" standalone=\"yes\"?>\n"
			position = line.find("=\"")
			subname = line[(position + 1):]
			subname = subname[1:]
			subname = subname[:-3]
			new_text = "	<language id= \"" + subname + "\">" + subname + ".xml</language>\n"
			newfile.write(new_text)
			subfile = subname + ".xml"
	        	print subfile
			subfile = cwd + "/" + subfile	
			sub_xml = open(subfile, "w")
			sub_xml.write(headline)
			line = line + '\n'
			sub_xml.write(line)
		
		if(line.find("</language>") >= 0) :
                	line = line + '\n'
			sub_xml.write(line)
			sub_xml.close
        	elif(line.find("<tran id=") >= 0) :
			sub_xml.write(line)
		elif(line.find("<font>") >= 0) :
			sub_xml.write(line)	

	newfile.write('</i18n>\n')
	newfile.close()

	newfile = open(newname, "r")
	
	xml_file.close()
	os.remove(file_path)
	xml_new = open(file_name, "w")
	
	line = newfile.readline()
	while line:
		xml_new.write(line)	
		line = newfile.readline()
	
	newfile.close()
	os.remove(newname)
	xml_new.close()
	
	
	
if __name__ == "__main__":
	split_xml()
