#include<bits/stdc++.h>
using namespace std;
class String
{
private:
    char* _ptr;
public:
    // 带默认参
    String(const char* ptr=nullptr)
    {
        // 防止一直需要判断是否为空，弄一个\0
        if(ptr==nullptr)
        {
            _ptr =new char[1];
            _ptr[0]='\0';   
        }
        else
        {
            _ptr=new char[strlen(ptr)];
            strcpy(_ptr,ptr);
        }
    }
    String(const String& s)
    {
        _ptr=new char[s.size()];
        strcpy(_ptr,s.get_ptr());
    }

    size_t size()const
    {
        return strlen(_ptr);
    }
    char* get_ptr()const
    {
        return _ptr;
    }

    
    String operator =(const String& s)
    {
        if(&s==this)
        {
            return *this;
        }
        delete []_ptr; 
        _ptr=new char[s.size()];
        strcpy(_ptr,s.get_ptr()); 
        return *this;
    }
    bool operator >(const String& s)const
    {
        return strcmp(_ptr,s._ptr)>0;
    }
    
    bool operator <(const String& s)const
    {
        return strcmp(_ptr,s._ptr)<0;
    }
     bool operator ==(const String& s)const
    {
        return strcmp(_ptr,s._ptr)<0;
    }
    char& operator[] (int index)
    {
        return _ptr[index];
    }
    const char& operator[] (int index) const
    {
        return _ptr[index];
    }
    //这个弄成友元的全局更好
    friend String operator +(const String& s1,const String& s2);
    friend ostream& operator<< (ostream& out,const String& s);
    friend istream& operator>> (istream& in,String& s);
    ~String()
    {
        delete [] _ptr;
        _ptr=nullptr;
    }
};

String operator +(const String& s1,const String& s2)
{
        // return 

        //此时有个很严重的问题,new了却没释放掉(貌似可以直接返回临时变量)
        String tmp;
        tmp._ptr=new char[s1.size()+s2.size()+1 ];
        strcpy(tmp._ptr,s1._ptr);
        strcat(tmp._ptr,s2._ptr);
        return tmp;
}

ostream& operator<< (ostream& out,const String& s)
{
    out<<s._ptr;
    return out;
}

istream& operator>> (istream& in,String& s)
{
    in>>s._ptr;
    return in;
}

int main()
{
    String s="asdad";
    String s2="asad";
    String s3;
    cout<<s3.size()<<endl;   //strlen不计算\0
    cout<<s2.get_ptr()<<endl;
    cout<<s+s2<<endl;
    cin.get();
}