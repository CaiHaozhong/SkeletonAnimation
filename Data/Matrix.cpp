#include "Matrix.h"
#include <assert.h>
const Matrix& Matrix::operator+=( const Matrix& m )
{
	if ( rows() != m.rows() || rows() != m.cols() )
	{
		return *this;
	}

	int r = rows();
	int c = cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			array[i][j] += m[i][j];
		}
	}

	return *this;
}


const Matrix& Matrix::operator-=( const Matrix& m )
{
	if ( rows() != m.rows() || cols() != m.cols() )
	{
		return *this;
	}

	int r = rows();
	int c = cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			array[i][j] -= m[i][j];
		}
	}

	return *this;
}

const Matrix& Matrix::operator*=( const Matrix& m )
{
	if ( cols() != m.rows() || !m.square() )
	{
		return *this;
	}

	Matrix ret( rows(), cols() );

	int r = rows();
	int c = cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			double sum = 0.0;
			for ( int k = 0; k < c; ++k )
			{
				sum += array[i][k] * m[k][j];
			}
			ret[i][j] = sum;
		}
	}

	*this = ret;
	return *this;
}

const Matrix& Matrix::operator/=( const Matrix& m )
{
	Matrix tmp = inverse( m );
	return operator*=( tmp );
}

void Matrix::set44( double a1, double a2, double a3, double a4, double b1, double b2, double b3, double b4, double c1, double c2, double c3, double c4, double d1, double d2, double d3, double d4 )
{
	this->resize(4,4);	
	array[0][0] = a1; array[0][1] = a2; array[0][2] = a3; array[0][3] = a4;
	array[1][0] = b1; array[1][1] = b2; array[1][2] = b3; array[1][3] = b4;
	array[2][0] = c1; array[2][1] = c2; array[2][2] = c3; array[2][3] = c4;
	array[3][0] = d1; array[3][1] = d2; array[3][2] = d3; array[3][3] = d4;
}

void Matrix::fromPoint3d( Point3d p )
{
	this->resize(4,1);
	array[0][0] = p.x;
	array[1][0] = p.y;
	array[2][0] = p.z;
	array[3][0] = 1;
}

Point3d Matrix::getPoint()
{
	return Point3d(array[0][0],array[1][0],array[2][0]);
}





bool operator==( const Matrix& lhs, const Matrix& rhs )
{
	if ( lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols() )
	{
		return false;
	}

	for ( int i = 0; i < lhs.rows(); ++i )
	{
		for ( int j = 0; j < lhs.cols(); ++j )
		{
			if ( rhs[i][j] != rhs[i][j] )
			{
				return false;
			}
		}
	}

	return true;
}

bool operator!=( const Matrix& lhs, const Matrix& rhs )
{
	return !( lhs == rhs );
}

const Matrix operator+( const Matrix& lhs, const Matrix& rhs )
{
	Matrix m;
	if ( lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols() )
	{
		return m;
	}

	m = lhs;
	m += rhs;

	return m;
}

const Matrix operator-( const Matrix& lhs, const Matrix& rhs )
{
	Matrix m;
	if ( lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols() )
	{
		return m;
	}

	m = lhs;
	m -= rhs;

	return m;
}

const Matrix operator*( const Matrix& lhs, const Matrix& rhs )
{
	Matrix m;
	if ( lhs.cols() != rhs.rows() )
	{
		return m;
	}

	m.resize( lhs.rows(), rhs.cols() );

	int r = m.rows();
	int c = m.cols();
	int K = lhs.cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			double sum = 0.0;
			for ( int k = 0; k < K; ++k )
			{
				sum += lhs[i][k] * rhs[k][j];
			}
			m[i][j] = sum;
		}
	}

	return m;
}

const Matrix operator/( const Matrix& lhs, const Matrix& rhs )
{
	Matrix tmp = inverse( rhs );
	Matrix m;

	if ( tmp.empty() )
	{
		return m;
	}

	return m = lhs * tmp;
}

inline static double LxAbs( double d )
{
	return (d>=0)?(d):(-d);
}

inline 
	static bool isSignRev( const vector<double>& v )
{
	int p = 0;
	int sum = 0;
	int n = (int)v.size();

	for ( int i = 0; i < n; ++i )
	{
		p = (int)v[i];
		if ( p >= 0 )
		{
			sum += p + i;
		}
	}

	if ( sum % 2 == 0 ) // 如果是偶数，说明不变号
	{
		return false;
	}
	return true;
}

// 计算方阵行列式
const double det( const Matrix& m )
{
	double ret = 0.0;

	if ( m.empty() || !m.square() ) return ret;

	Matrix N = LU( m );

	if ( N.empty() ) return ret;

	ret = 1.0;
	for ( int i = 0; i < N.cols(); ++ i )
	{
		ret *= N[i][i];
	}

	if ( isSignRev( N[N.rows()-1] ))
	{
		return -ret;
	}

	return ret;
}

// 计算矩阵指定子方阵的行列式 
const double det( const Matrix& m, int start, int end )
{
	return det( submatrix(m, start, end, start, end) );
}


// 计算矩阵转置
const Matrix trans( const Matrix& m )
{
	Matrix ret;
	if ( m.empty() ) return ret;

	int r = m.cols();
	int c = m.rows();

	ret.resize(r, c);
	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			ret[i][j] = m[j][i];
		}
	}

	return ret;
}

// 计算逆矩阵
const Matrix  inverse( const Matrix& m ) 
{
	Matrix ret;

	if ( m.empty() || !m.square() )
	{
		return ret;
	}

	int n = m.rows();

	ret.resize( n, n );
	Matrix A(m);

	for ( int i = 0; i < n; ++i ) ret[i][i] = 1.0;

	for ( int j = 0; j < n; ++j )  //每一列
	{
		int p = j;
		double maxV = LxAbs(A[j][j]);
		for ( int i = j+1; i < n; ++i )  // 找到第j列中元素绝对值最大行
		{
			if ( maxV < LxAbs(A[i][j]) )
			{
				p = i;
				maxV = LxAbs(A[i][j]);
			}
		}

		if ( maxV < 1e-20 ) 
		{
			ret.resize(0,0);
			return ret;
		}

		if ( j!= p )
		{
			A.swap_row( j, p );
			ret.swap_row( j, p );
		}

		double d = A[j][j];
		for ( int i = j; i < n; ++i ) A[j][i] /= d;
		for ( int i = 0; i < n; ++i ) ret[j][i] /= d;

		for ( int i = 0; i < n; ++i )
		{
			if ( i != j )
			{
				double q = A[i][j];
				for ( int k = j; k < n; ++k )
				{
					A [i][k] -= q * A[j][k];
				}
				for ( int k = 0; k < n; ++k )
				{
					ret[i][k] -= q * ret[j][k];
				}
			}
		}
	}

	return ret;
}

// 计算绝对值
const Matrix abs( const Matrix& m )
{
	Matrix ret;

	if( m.empty() )
	{
		return ret;
	}

	int r = m.rows();
	int c = m.cols();
	ret.resize( r, c );

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			double t = m[i][j];
			if ( t < 0 ) ret[i][j] = -t;
			else ret[i][j] = t;
		}
	}

	return ret;
}

// 返回矩阵所有元素的最大值
const double max( const Matrix& m )
{
	if ( m.empty() ) return 0.;

	double ret = m[0][0];
	int r = m.rows();
	int c = m.cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			if ( m[i][j] > ret ) ret = m[i][j];
		}
	}
	return ret;
}

// 计算矩阵最大值，并返回该元素的引用
const double max( const Matrix& m, int& row, int& col )
{
	if ( m.empty() ) return 0.;

	double ret = m[0][0];
	row = 0;
	col = 0;

	int r = m.rows();
	int c = m.cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			if ( m[i][j] > ret )
			{
				ret = m[i][j];
				row = i;
				col = j;
			}
		}
	}
	return ret;
}

// 计算矩阵所有元素最小值
const double min( const Matrix& m )
{
	if ( m.empty() ) return 0.;

	double ret = m[0][0];
	int r = m.rows();
	int c = m.cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			if ( m[i][j] > ret ) ret = m[i][j];
		}
	}

	return ret;
}

// 计算矩阵最小值，并返回该元素的引用
const double min( const Matrix& m, int& row, int& col)
{
	if ( m.empty() ) return 0.;

	double ret = m[0][0];
	row = 0;
	col = 0;
	int r = m.rows();
	int c = m.cols();

	for ( int i = 0; i < r; ++i )
	{
		for ( int j = 0; j < c; ++j )
		{
			if ( m[i][j] > ret )
			{
				ret = m[i][j];
				row = i;
				col = j;
			}
		}
	}

	return ret;
}

// 取矩阵中指定位置的子矩阵 
const Matrix submatrix(const Matrix& m,int rb,int re,int cb,int ce)
{
	Matrix ret;
	if ( m.empty() ) return ret;

	if ( rb < 0 || re >= m.rows() || rb > re ) return ret;
	if ( cb < 0 || ce >= m.cols() || cb > ce ) return ret;

	ret.resize( re-rb+1, ce-cb+1 );

	for ( int i = rb; i <= re; ++i )
	{
		for ( int j = cb; j <= ce; ++j )
		{
			ret[i-rb][j-cb] = m[i][j];
		}
	}

	return ret;
}


inline static
	int max_idx( const Matrix& m, int k, int n )
{
	int p = k;
	for ( int i = k+1; i < n; ++i )
	{
		if ( LxAbs(m[p][k]) < LxAbs(m[i][k]) )
		{
			p = i;
		}
	}
	return p;
}

// 计算方阵 M 的 LU 分解
// 其中L为对角线元素全为1的下三角阵，U为对角元素依赖M的上三角阵
// 使得 M = LU
// 返回矩阵下三角部分存储L(对角元素除外)，上三角部分存储U(包括对角线元素)
const Matrix LU( const Matrix& m )
{
	Matrix ret;

	if ( m.empty() || !m.square() ) return ret;

	int n = m.rows();
	ret.resize( n+1, n );

	for ( int i = 0; i < n; ++i )
	{
		ret[n][i] = -1.0;
	}

	for ( int i = 0; i < n; ++i )
	{
		for ( int j = 0; j < n; ++j )
		{
			ret[i][j] = m[i][j];
		}
	}

	for ( int k = 0; k < n-1; ++k )
	{
		int p = max_idx( ret, k, n );
		if ( p != k )              // 进行行交换
		{
			ret.swap_row( k, p );
			ret[n][k] = (double)p; // 记录将换信息
		}

		if ( ret[k][k] == 0.0 )
		{
			assert(true);
			ret.resize(0,0);
			return ret;
		}

		for ( int i = k+1; i < n; ++i )
		{
			ret[i][k] /= ret[k][k];
			for ( int j = k+1; j < n; ++j )
			{
				ret[i][j] -= ret[i][k] * ret[k][j];
			}
		}
	}

	return ret;
}
