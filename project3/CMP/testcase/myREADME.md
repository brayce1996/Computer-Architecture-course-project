1. 要先建好golden 和 你的simulator link在這個資料夾
    ln -s {path/of/golden} go_sim
    ln -s {path/of/your/sim} my_sim

2. 在這個資料夾創一個名為"wrong"的資料夾
3. 把dMemWriter , assembler , Ran.sh , cmp.sh , random 的執行權限都開起來
    chmod 700 dMemWriter
    chmod 700 assembler
    chmod 700 Ran.sh
    chmod 700 cmp.sh
    chmod 700 random
    
4. 然後./Ran.sh
5. 去喝杯水，如果有錯的測資，會列在wrong中