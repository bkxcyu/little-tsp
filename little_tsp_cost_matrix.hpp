#ifndef LITTLE_TSP_COST_MATRIX_H
#define LITTLE_TSP_COST_MATRIX_H

#include <vector>

#include "matrix.hpp"

class Graph;
class CostMatrixInteger;
struct Edge;

// information about the useable matrix
// temporary structure that is used to help build a TreeNode
class CostMatrix {
public:
	CostMatrix(const Graph& graph, const std::vector<Edge>& include, 
			const std::vector<Edge>& exclude);

	int ReduceMatrix();

	CostMatrixInteger operator()(int row_num, int column_num) const;

	bool IsRowAvailable(int row_num) const { return row_available_[row_num]; }
	bool IsColumnAvailable(int column_num) const { 
		return column_available_[column_num]; 
	}
	int size() const { return cost_matrix_.size().first; }

	class CostVector;
	class CostRow; 
	class CostColumn;
	// these classes need to be able to share data with each other because they
	// are very intimately related
	friend class CostVector;

	CostRow GetRow(int row_num) const;
	CostColumn GetColumn(int column_num) const;

	// encapsulation of cost information about either a single row or a column
	// defines iterator which allows easier traversal and more use of STL
	class CostVector {
	public:
		CostVector(const Matrix<CostMatrixInteger>& cost_matrix) :
				cost_matrix_{cost_matrix} {}

		int operator[](int cell_num) const;

		// we need access to the protected interface in cost vector and the
		// subscripting operation
		class Iterator;
		friend class Iterator;

		class Iterator {
		public:
			Iterator(const CostVector& cost_vector) :
				cost_vector_{cost_vector}, traversing_cell_index_{0} {}

			CostMatrixInteger operator*() { 
				return cost_vector_[traversing_cell_index_]; 
			}

			CostMatrixInteger* operator*() { 
				return &cost_vector_[traversing_cell_index_]; 
			}

			Iterator operator++(int);
			Iterator& operator++();

			bool operator==(Iterator& other);
			bool operator!=(Iterator& other);

			friend class CostVector;

		private:
			Iterator(const CostVector& cost_vector, int cell_num) :
				cost_vector_{cost_vector}, traversing_cell_index_{cell_num} {}

			const CostVector& cost_vector_;
			int traversing_cell_index_;  // changes as iterator moves
		};

		Iterator begin() { return Iterator{*this}; }
		Iterator end() { 
			return Iterator{*this, cost_matrix_.cost_matrix_.size()}; 
		}

	protected:
		// define interface for derived classes because CostVector doesn't know
		// if it's a row or column
		virtual int GetRow(int cell_num) const = 0;
		virtual int GetColumn(int cell_num) const = 0;

		bool IsRowAvailable(int row_num) { 
			return cost_matrix_.IsRowAvaiable(row_num); 
		}
		bool IsColumnAvailable(int column_num) { 
			return cost_matrix_.IsColumnAvailable(column_num); 
		}

	private:
		const CostMatrix& cost_matrix_;
	};

	class CostColumn : public CostVector {
	public:
		CostColumn(const Matrix<CostMatrixInteger>& cost_matrix, 
				int column_num) : CostVector{cost_matrix}, 
				column_num_{column_num} {
			if (IsColumnAvailable(column_num_)) {
				throw NotAvailableError{"That column is not available"};
			}
		}

	private:
		int GetRow(int cell_num) const override { return cell_num; }
		int GetColumn(int) const override { return column_num_; }

		int column_num_;
	};

	class CostRow : public CostVector {
	public:
		CostRow(const CostMatrix& cost_matrix_, int row_num) :
				CostVector{cost_matrix_}, row_num_{row_num} {
			if (IsRowAvailable(row_num_)) {
				throw NotAvailableError{"That row is not available"};
			}
		}

	private:
		int GetColumn(int cell_num) const override { return cell_num; }
		int GetRow(int) const override { return row_num_; }

		int row_num_;
	};

	// we need access to the protected interface in cost matrix and the
	// subscripting operation
	class Iterator;
	friend class Iterator;

	// iterator for iterating over all cells in the cost matrix
	class Iterator {
	public:
		Iterator(const CostMatrix& cost_matrix) :
			cost_matrix_{cost_matrix}, row_num_{0}, column_num_{0} {}

		CostMatrixInteger operator*() { 
			return cost_matrix_(row_num_, column_num_);
		}

		CostMatrixInteger* operator->() {
			return &cost_matrix_(row_num_, column_num_);
		}

		Iterator operator++(int);
		Iterator& operator++();

		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);

		friend class CostMatrix;

	private:
		Iterator(const CostMatrix& cost_matrix, int row_num, int column_num) :
			cost_matrix_{cost_matrix}, row_num_{row_num}, 
			column_num_{column_num} {}

		void MoveToNextCell();

		const CostMatrix& cost_matrix_;
		int row_num_;  // changes as iterator moves
		int column_num_;  // changes as iterator moves
	};

	Iterator begin() { return Iterator{*this}; }
	Iterator end() { 
		return Iterator{*this, cost_matrix_.size()}; 
	}
	
private:
	Matrix<CostMatrixInteger> cost_matrix_;
	std::vector<bool> row_available_;
	std::vector<bool> column_available_;
};


#endif  // LITTLE_TSP_COST_MATRIX_H
