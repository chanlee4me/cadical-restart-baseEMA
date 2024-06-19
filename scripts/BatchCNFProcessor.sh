#仅处理指定文件个数
cd  /home/wgf/chenli/SAT/cnf
processed_count=$(grep -c '^/home' /home/wgf/chenli/SAT/real_ori_result.csv) # count the number of lines in result.csv that start with /home
total_files=250 # total number of files you want to process
remaining_files=$((total_files - processed_count)) # calculate the remaining number of files

count=0
for i in $( ls *.cnf) # for each cnf file in 'formula' folder
do
 if [ $count -ge $remaining_files ]; then
    echo "Processed $remaining_files files, stopping"
    break
 fi

 str=`readlink -f $i` 
 echo "begin $str"
 
 # Check if the file is already in the CSV
 if grep -q "$str" /home/wgf/chenli/SAT/real_ori_result.csv; then
    echo "$str already processed, skipping"
    continue
 fi

 printf "%s," $str >>/home/wgf/chenli/SAT/real_ori_result.csv #record the absolutly cnf file path
 timeout 1200  /home/wgf/chenli/SAT/restart_strategy/restartCadical/raw_cadical/build/cadical $str |awk -F "[{}]"  '/statistics/{flag1=1;next} flag1{print $0; if(++n1==29) flag1=0} /resources/{flag2=1;next} flag2{print $0; if(++n2==6) exit}'   >>/home/wgf/chenli/SAT/real_ori_result.csv
 printf "\n" >>/home/wgf/chenli/SAT/real_ori_result.csv
 echo "end $str"

 count=$((count+1))
done

#完整处理CNF文件
# cd  /home/chanlee/SAT/cnf_2022
# for i in $( ls *.cnf) # for each cnf file in 'formula' folder
# do
#  str=`readlink -f $i` 
#  echo "begin $str"
 
#  # Check if the file is already in the CSV
#  if grep -q "$str" ../result.csv; then
#     echo "$str already processed, skipping"
#     continue
#  fi

#  printf "%s," $str >>../result.csv #record the absolutly cnf file path
#  timeout 1200  /home/chanlee/SAT/Restart_CADICAL-1.4.1/cadical/build/cadical $str |awk -F "[{}]"  '/statistics/{flag1=1;next} flag1{print $0; if(++n1==26) flag1=0} /resources/{flag2=1;next} flag2{print $0; if(++n2==6) exit}'   >>../result.csv
#  printf "\n" >>../result.csv
#  echo "end $str"
# done
