typedef struct packed { bit [0:3]   cache_type;
                        bit [0:15]  index;
                        bit [0:3]   way;
                        bit [0:3]   accessed;
                        bit [0:7]   lru; } c_asser_mem; //bit feilds are according to assertiion_dat.txt file 

//`define DEBUG; 

`ifdef DEBUG
`define D if (1)
`else
`define D if (0)
`endif

module top();


c_asser_mem Mem[$];
bit [0:3] dlru[int][int][$];
bit [0:3] ilru[int][int][$];
bit daccessed[int][int][$];
bit iaccessed[int][int][$];


initial
begin
$readmemh("assertion_dat.txt",Mem);
//foreach(Mem[i])
    //$display("cache_type:-%h index:-%4h way:-%1h accessed:- %b LRU:-%2h",Mem[i].cache_type,Mem[i].index,Mem[i].way,Mem[i].accessed,Mem[i].lru);
    foreach(Mem[i])
    begin
    if(Mem[i].cache_type)
        begin
            dlru[Mem[i].index][Mem[i].way].push_back(Mem[i].lru);
            daccessed[Mem[i].index][Mem[i].way].push_back(Mem[i].accessed);
        end
    else
        begin
            ilru[Mem[i].index][Mem[i].way].push_back(Mem[i].lru);
            iaccessed[Mem[i].index][Mem[i].way].push_back(Mem[i].accessed);
        end
    end

`D 
begin
foreach(dlru[i,j])
        $display("index:- %4h way:- %1h lru_queue:- %p",i,j,dlru[i][j]);
end

    foreach(daccessed[x,y,k])
    begin
    automatic bit flag[int];    
    foreach(daccessed[i,j,h])
    begin
    flag[i] = 0;
    if(h === 0)
        begin
            automatic bit check_lru = daccessed[i][j].pop_front();
            automatic bit [0:3] lru = dlru[i][j].pop_front();
            if(check_lru)
                begin
                flag[i] = 1;
                assert(lru === 4'h7);
                end
            else if(daccessed[i][j][h] !== 1'b1 && flag[i] == 1)
                begin
                assert(lru === dlru[i][j][h] + 1'b1);
                end
            `D $display("accessed lru:- %1h next_lru :- %1h lru:- %1h index :- %4h way :- %1h",check_lru,dlru[i][j][h],lru,i,j);//"%4h %1h", i, j);
        end  
    end
    end

    
end

endmodule
