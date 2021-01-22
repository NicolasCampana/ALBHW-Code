#pragma once

#ifndef NICOLAS_IC_ALBHWMODELRESULTS_H
#define NICOLAS_IC_ALBHWMODELRESULTS_H
class ResultModel : public GRBCallback {
public:
	int funObjet;
	double gap;
	double time;
	int status;
	int *workers;
	double lower;
	double lowerRoot;
	double nodes;
	bool flag = false;
protected:
    void callback() {
        try {
            if (where == GRB_CB_MIPNODE && !flag) {
                if(getDoubleInfo(GRB_CB_MIPNODE_NODCNT) == 0) {
                    this->lowerRoot = getDoubleInfo(GRB_CB_MIPNODE_OBJBNDC);
                } else {
                    flag = true;
                }
            }
        } catch (GRBException e) {
            cout << "Error number: " << e.getErrorCode() << endl;
            cout << e.getMessage() << endl;
        }
    }
};

#endif



