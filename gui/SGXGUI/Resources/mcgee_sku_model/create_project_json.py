import pandas as pd
import json
import xml.etree.ElementTree as ET
import os

sdt_name = './dark-mode-model - Copy.sdt'
csv_name = './Mcgee and co - Sheet1.csv'

files = [sdt_name, csv_name]

table = ''
x = ''
y = ''
z = ''

tree = ET.parse(sdt_name)
root = tree.getroot()

for item in root.findall('Datasources'):
	for child in item:
		table = child.attrib['id']

x_id = ''
y_id = ''
z_id = ''

for item in root.findall('BaseObjects'):
	for child in item:
		if 'filename' in child.attrib:
			img = os.path.basename(child.attrib['filename'])
			if img not in files:
				files.append(img)

for item in root.findall('Glyphs'):
	for child in item:
		pos = child.find('Position')
		x_id = pos.find('X').find('Binding').attrib['fieldId']
		y_id = pos.find('Y').find('Binding').attrib['fieldId']
		z_id = pos.find('Z').find('Binding').attrib['fieldId']

for item in root.findall('InputFields'):
	for child in item:
		name = child.attrib['name']
		if name == x_id:
			x = child.attrib['field']
		elif name == y_id:
			y = child.attrib['field']
		elif name == z_id:
			z = child.attrib['field']

df = pd.read_csv(csv_name, keep_default_na=False)

output = {'files': files,'id': table,'properties': {'x':x,'y':y,'z':z}, 'columns': []}

#types = object, int64, float64, bool, datetime64, timedelta[ns], category
for name, dtype in df.dtypes.iteritems():
	if dtype == 'object':
		_sorted = df.sort_values(by=[name])
		_min = _sorted.iloc[0,:][name]
		_max = _sorted.iloc[-1][name]
	elif dtype == 'int64':
		_max = int(df[name].loc[df[name].idxmax()])      # Maximum in column
		_min = int(df[name].loc[df[name].idxmin()])      # Minimum in column
	else:
		_max = df[name].loc[df[name].idxmax()]      # Maximum in column
		_min = df[name].loc[df[name].idxmin()]      # Minimum in column
	output['columns'].append({'name': name, 'min': _min, 'max': _max})

with open("sidebar.json", "w") as outfile:
    json.dump(output, outfile)
