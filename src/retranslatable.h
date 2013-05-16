#ifndef RETRANSLATABLE_H
#define RETRANSLATABLE_H

#include <QList>

class Retranslatable;
static QList<Retranslatable*> retranslatableWidgets;

class Retranslatable
{ 
public:
     Retranslatable(){retranslatableWidgets.append(this);}
     virtual ~Retranslatable(){retranslatableWidgets.removeAll(this);}
     static void retranslateAll(){
	  for(int i=0;i<retranslatableWidgets.size();i++)
	       retranslatableWidgets[i]->retranslate();
     }
     virtual void retranslate() = 0;
};




#endif
