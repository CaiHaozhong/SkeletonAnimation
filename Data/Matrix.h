#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <vector>
#include <string>
#include "Point3d.h"
using std::vector;
using std::string;

// �������;�����
template <typename Object>
class MATRIX
{
public:
	explicit MATRIX() : array( 0 ) {}

	MATRIX( int rows, int cols):array( rows )
	{
		for( int i = 0; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
	
	MATRIX( const MATRIX<Object>& m ){ *this = m;}

	void resize( int rows, int cols );           // �ı䵱ǰ�����С
	bool push_back( const vector<Object>& v );   // �ھ���ĩβ���һ������
	void swap_row( int row1, int row2 );         // �������е�����

	int  rows() const{ return array.size(); }
	int  cols() const { return rows() ? (array[0].size()) : 0; }
	bool empty() const { return rows() == 0; }        // �Ƿ�Ϊ��
	bool square() const { return (!(empty()) && rows() == cols()); }  // �Ƿ�Ϊ����


	const vector<Object>& operator[](int row) const { return array[row]; } //[]���������� 
	vector<Object>& operator[](int row){ return array[row]; }

protected:
	vector< vector<Object> > array;
};

// �ı䵱ǰ�����С
template <typename Object>
void MATRIX<Object>::resize( int rows, int cols )
{
	int rs = this->rows();
	int cs = this->cols();

	if ( rows == rs && cols == cs )
	{
		return;
	}
	else if ( rows == rs && cols != cs )
	{
		for ( int i = 0; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
	else if ( rows != rs && cols == cs )
	{
		array.resize( rows );
		for ( int i = rs; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
	else
	{
		array.resize( rows );
		for ( int i = 0; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
}

// �ھ���ĩβ���һ��
template <typename Object>
bool MATRIX<Object>::push_back( const vector<Object>& v )
{
	if ( rows() == 0 || cols() == (int)v.size() )
	{
		array.push_back( v );
	}
	else
	{
		return false;
	}

	return true;
}

// ��������
template <typename Object>
void MATRIX<Object>::swap_row( int row1, int row2 )
{
	if ( row1 != row2 && row1 >=0 &&
		row1 < rows() && row2 >= 0 && row2 < rows() )
	{
		vector<Object>& v1 = array[row1];
		vector<Object>& v2 = array[row2];
		vector<Object> tmp = v1;
		v1 = v2;
		v2 = tmp;
	}
}

// ����ת��
template <typename Object>
const MATRIX<Object> trans( const MATRIX<Object>& m )
{
	MATRIX<Object> ret;
	if ( m.empty() ) return ret;

	int row = m.cols();
	int col = m.rows();
	ret.resize( row, col );

	for ( int i = 0; i < row; ++i )
	{
		for ( int j = 0; j < col; ++j )
		{
			ret[i][j] = m[j][i];
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////
// double���;����࣬���ڿ�ѧ����
// �̳���MATRIX��
// ʵ�ֳ��ò��������أ���ʵ�ּ�����������ʽ�����Լ�LU�ֽ�
class Matrix:public MATRIX<double>
{
public:
	Matrix():MATRIX<double>(){}
	Matrix( int c, int r ):MATRIX<double>(c,r){}
	Matrix( const Matrix& m){ *this  = m; }
	void set44(double a1, double a2, double a3, double a4, 
		double b1, double b2, double b3, double b4,
		double c1, double c2, double c3, double c4,
		double d1, double d2, double d3, double d4
		);
	void fromArray(double arr[16])
	{
		set44(arr[0],arr[1],arr[2],arr[3],
			arr[4],arr[5],arr[6],arr[7],
			arr[8],arr[9],arr[10],arr[11],
			arr[12],arr[13],arr[14],arr[15]
			);
	}
	void fromPoint3d(Point3d p);
	Point3d getPoint();
	const Matrix& operator+=( const Matrix& m );
	const Matrix& operator-=( const Matrix& m );
	const Matrix& operator*=( const Matrix& m );
	const Matrix& operator/=( const Matrix& m );
};

bool  operator==( const Matrix& lhs, const Matrix& rhs );        // ���ز�����==
bool  operator!=( const Matrix& lhs, const Matrix& rhs );        // ���ز�����!=
const Matrix operator+( const Matrix& lhs, const Matrix& rhs );  // ���ز�����+
const Matrix operator-( const Matrix& lhs, const Matrix& rhs );  // ���ز�����-
const Matrix operator*( const Matrix& lhs, const Matrix& rhs );  // ���ز�����*
const Matrix operator/( const Matrix& lhs, const Matrix& rhs );  // ���ز�����/
const double det( const Matrix& m );                             // ��������ʽ
const double det( const Matrix& m, int start, int end );         // �����Ӿ�������ʽ
const Matrix abs( const Matrix& m );                             // ��������Ԫ�صľ���ֵ
const double max( const Matrix& m );                             // ����Ԫ�ص����ֵ
const double max( const Matrix& m, int& row, int& col);          // ����Ԫ���е����ֵ�����±�
const double min( const Matrix& m );                             // ����Ԫ�ص���Сֵ
const double min( const Matrix& m, int& row, int& col);          // ����Ԫ�ص���Сֵ�����±�
const Matrix trans( const Matrix& m );                           // ����ת�þ���
const Matrix submatrix(const Matrix& m,int rb,int re,int cb,int ce);  // �����Ӿ���
const Matrix inverse( const Matrix& m );                         // ���������
const Matrix LU( const Matrix& m );                              // ���㷽���LU�ֽ�

#endif
