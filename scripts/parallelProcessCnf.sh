
#cl
#仅处理指定文件个数
cd  /home/wgf/chenli/SAT/cnf
processed_count=$(grep -c '^/home' /home/wgf/chenli/SAT/restart_result.csv) # count the number of lines in result.csv that start with /home
total_files=250 # total number of files you want to process
remaining_files=$((total_files - processed_count)) # calculate the remaining number of files

count=0
ls *.cnf | xargs -n 3 -P 192 -I {} bash -c '
 if [ $0 -ge $1 ]; then
    echo "Processed $1 files, stopping"
    exit
 fi

 str=`readlink -f {}` 
 echo "begin $str"
 
 # Check if the file is already in the CSV
 if grep -q "$str" /home/wgf/chenli/SAT/baseEMA-restart_result.csv; then
    echo "$str already processed, skipping"
    exit
 fi

 temp_file=$(mktemp)
 printf "%s," $str >>$temp_file #record the absolutly cnf file path
 timeout 3600  /home/wgf/chenli/SAT/cadical-restart-baseEMA/build/cadical -l $str |awk -F "[{}]"  "/statistics/{flag1=1;next} flag1{print \$0; if(++n1==29) flag1=0} /resources/{flag2=1;next} flag2{print \$0; if(++n2==6) exit}"   >>$temp_file
 printf "\n" >>$temp_file
 echo "end $str"

 mv $temp_file /home/wgf/chenli/SAT/baseEMA-restart_result.csv.$BASHPID
' $count $remaining_files

# Wait for all parallel jobs to finish
wait

# Merge all temporary files
cat /home/wgf/chenli/SAT/baseEMA-restart_result.csv.* >> /home/wgf/chenli/SAT/baseEMA-restart_result.csv

# Remove temporary files
rm /home/wgf/chenli/SAT/baseEMA-restart_result.csv.*