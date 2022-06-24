rm file.txt
make &> file.txt < file.txt
sed '/Applications/d' file.txt > file2.txt
sed '/operator/d' file2.txt > file3.txt
rm file.txt file2.txt
mv file3.txt file.txt