#This script gets called every day at 5 in the morning to fetch the database from s3.
#This script downloads a zip file and then extracts it.
#!/bin/bash

echo "" 
echo "" 
echo "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////" 
echo "--- Running script at $(date) ---" 
echo "" 
echo "" 

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ORIGIN="s3://visualizationdata/"
InstFolderName="Resources/Institutions"
echo Directory: $DIR

if [ ! -d $DIR/$InstFolderName ]; then
    mkdir -p $DIR/$InstFolderName
fi

#Sync the s3 directory with ec2 instance.
aws s3 --quiet sync $ORIGIN $DIR/$InstFolderName --exclude "guide.zip"

for dirname in $(ls $DIR/$InstFolderName);
do

echo $dirname

if [ ! -f "$DIR/$InstFolderName/$dirname/data.zip" ]; then
	echo "Not even a single zip file in the directory"
	continue
fi
	

unzip -q -o "$DIR/$InstFolderName/$dirname/*.zip" -d "$DIR/$InstFolderName/$dirname"
rm $DIR/$InstFolderName/$dirname/*.zip

done

java -cp $DIR/DataEngine/classes/ UpdateDataSourcePaths "$DIR/$InstFolderName"

echo "" 
echo "" 
echo "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////" 
echo "" 
echo "" 