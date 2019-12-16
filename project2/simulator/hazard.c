#include"hazard.h"

int dataHazardDetect(){
    //--init--//
    instruction *ID = getID();
    instruction *EX = getEX();
    instruction *MEM = getMEM();
    instruction *WB = getWB();
    
    int ID_wbReg;
    int EX_wbReg;
    int MEM_wbReg;
    
    if(ID->wbFlag==1) ID_wbReg = ID->rd;
    else if(ID->wbFlag==2)  ID_wbReg = ID->rt;
    else ID_wbReg = -1;
    
    if(EX->wbFlag==1) EX_wbReg = EX->rd;
    else if(EX->wbFlag==2)  EX_wbReg = EX->rt;
    else EX_wbReg = -1;
    
    if(MEM->wbFlag==1) MEM_wbReg = MEM->rd;
    else if(MEM->wbFlag==2)  MEM_wbReg = MEM->rt;
    else MEM_wbReg = -1;
    
    if(MEM_wbReg!=-1&&MEM_wbReg!=0){
        if(EX->rs==MEM_wbReg){
            EX->forwardFlag_rs = 1;
            EX->rsValue = MEM->result;
        }
        if((EX->wbFlag==1||EX->wbFlag==3)&&EX->rt==MEM_wbReg){  //R mode only
            EX->forwardFlag_rt = 1;
            EX->rtValue = MEM->result;
        }
        if(MEM_wbReg==ID->rs&&(EX_wbReg==-1||EX_wbReg!=ID->rs)) ID->stallFlag = 1;
        if(ID->opcode==0x00&&(ID->func==0x00||ID->func==0x02||ID->func==0x03)){
            ID->stallFlag=0;
        }
        
        
        if((ID->wbFlag==1||ID->wbFlag==3)&&MEM_wbReg==ID->rt&&(EX_wbReg==-1||EX_wbReg!=ID->rt)) ID->stallFlag = 1;
    
    }
    
    
    //--check stall--//--check ID--//--for beq/bne/bgtz--//--for load operation--//
    //ID stall if ID is beq/bne/bgtz
    //         or EX is load operation.
    //         where EX->rd/rt == ID->rt/rs
    
    //--ID stage stall detect and forwording--//
    if(ID->opcode==0x04||ID->opcode==0x05){     //beq or bne
        ID->stallFlag = 0;
        if(EX->nopFlag==0&&EX_wbReg!=0&&EX->wbFlag!=-1&&(EX_wbReg==ID->rs||EX_wbReg==ID->rt)) ID->stallFlag = 1;
        else if(MEM_wbReg!=0&&MEM->wbFlag!=-1){
            if(MEM_wbReg==ID->rs) {
                if(MEM->opcode==0x23||MEM->opcode==0x21||MEM->opcode==0x25||MEM->opcode==0x20||MEM->opcode==0x24)
                    ID->stallFlag = 1;
                else{
                    ID->forwardFlag_rs = 1;
                    ID->rsValue = MEM->result;
                }
            }
            if(MEM_wbReg==ID->rt) {
                if(MEM->opcode==0x23||MEM->opcode==0x21||MEM->opcode==0x25||MEM->opcode==0x20||MEM->opcode==0x24)
                    ID->stallFlag = 1;
                else {
                    ID->forwardFlag_rt = 1;
                    ID->rtValue = MEM->result;
                }
            }
        }
    }
    else if(ID->opcode==0x07||(ID->opcode==0x00&&ID->func==0x08)) { //bgtz or jr
        ID->stallFlag=0;
        if(EX->nopFlag==0&&EX_wbReg!=0&&EX->wbFlag!=-1&&EX_wbReg==ID->rs) ID->stallFlag = 1;
        else if(MEM_wbReg!=0&&MEM->wbFlag!=-1&&MEM_wbReg==ID->rs){
            if(MEM->opcode==0x23||MEM->opcode==0x21||MEM->opcode==0x25||MEM->opcode==0x20||MEM->opcode==0x24)
                ID->stallFlag = 1;
            else{    
                ID->forwardFlag_rs = 1;
                ID->rsValue = MEM->result;
            }
        }
    }
        
    //--load stall deteciton--//
    if(EX_wbReg!=0&&(EX->opcode==0x23||EX->opcode==0x21||EX->opcode==0x25||EX->opcode==0x20||EX->opcode==0x24)){
        if(ID->rs==EX_wbReg) ID->stallFlag = 1;
        if((ID->wbFlag==1||ID->wbFlag==3)&&ID->rt==EX_wbReg) ID->stallFlag = 1;
        
    }
    
    //--nop/lui/halt should never be forwarded or stalled--//
    if(ID->nopFlag||ID->opcode==0x0F||ID->opcode==0x3F){
        ID->forwardFlag_rs=0;
        ID->forwardFlag_rt=0;
        ID->stallFlag=0;
    }
    if(EX->opcode==0x0F||EX->opcode==0x3F){
        EX->forwardFlag_rs=0;
        EX->forwardFlag_rt=0;
        EX->stallFlag=0;
    }
    
    //--sll/srl/sra should not forward to rs--//
    
    if(EX->opcode==0x00&&(EX->func==0x00||EX->func==0x02||EX->func==0x03)){
        EX->forwardFlag_rs=0;
    }
    
    return 0;
}

/**
 * some issues:
 * 1.   add $0 $s0 $s1
 *      add $0 $s0 $s0  //should not stall
 *                      //because of write $0 error occur
 * 
 * 2.     
 * 
 * 
 * */
 
 
/**
 * 
 * @return      if need to flush IF ,return 1;
 *              or return 0.
 * */
int ctrlHazardDetect(){
    return jumpDetect(getID());
}