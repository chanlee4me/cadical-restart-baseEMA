#updated by cl 2024/6/26 
cd /home/wgf/chenli/SAT/2022cnf
processed_files="/home/wgf/chenli/SAT/baseEMA-restart_result.csv"
total_files=400

# 获取系统的CPU核心数
num_cores=$(nproc)

# 生成未处理文件的列表
find . -name "*.cnf" | while read file; do
    if ! grep -q "$(readlink -f "$file")" "$processed_files"; then
        echo "$file"
    fi
done > /tmp/unprocessed_files.txt

# 只处理指定数量的文件，使用系统核心数作为并行数
head -n $total_files /tmp/unprocessed_files.txt | xargs -n 1 -P $num_cores -I {} bash -c '
    file="{}"
    str=$(readlink -f "$file")
    echo "begin $str"
    
    temp_file=$(mktemp)
    printf "%s," "$str" >> "$temp_file"
    timeout 3600 /home/wgf/chenli/SAT/cadical-restart-baseEMA/build/cadical "$str" | awk -F "[{}]" "/statistics/{flag1=1;next} flag1{print \$0; if(++n1==29) flag1=0} /resources/{flag2=1;next} flag2{print \$0; if(++n2==6) exit}" >> "$temp_file"
    printf "\n" >> "$temp_file"
    echo "end $str"
    
    mv "$temp_file" "/home/wgf/chenli/SAT/baseEMA-restart_result.csv.$BASHPID"
'

# 合并所有临时文件到一个csv文件中
for tmp_file in /home/wgf/chenli/SAT/baseEMA-restart_result.csv.*; do
    cat "$tmp_file" >> "/home/wgf/chenli/SAT/baseEMA-restart_result.csv"
    rm "$tmp_file" # 删除临时文件
done