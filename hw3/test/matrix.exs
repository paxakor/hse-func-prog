defmodule Matrix do
  @on_load :init

  def init do
    :ok = :erlang.load_nif("../build/matrix", 0)
  end

  def listToMatrix(_arg) do
    exit(:nif_not_loaded)
  end

  def listToVector(_arg) do
    exit(:nif_not_loaded)
  end

  def matrixToList(_arg) do
    exit(:nif_not_loaded)
  end

  def vectorToList(_arg) do
    exit(:nif_not_loaded)
  end

  def matrixMult(_matrix, _scalarOrMatrix) do
    exit(:nif_not_loaded)
  end

  def matrixVectorMult(_matrix, _vector) do
    exit(:nif_not_loaded)
  end

  def matrixSum(_lhs, _rhs) do
    exit(:nif_not_loaded)
  end

  def getRow(_matrix, _n) do
    exit(:nif_not_loaded)
  end

  def getColumn(_matrix, _n) do
    exit(:nif_not_loaded)
  end

  def getDiag(_matrix, _n) do
    exit(:nif_not_loaded)
  end

  def transposeRowToColumn(_row) do
    exit(:nif_not_loaded)
  end
end

IO.puts "test matrix"
m = [[1.0, 2.0], [3.0, 4.0], [5.0, 9.0]]
t = Matrix.listToMatrix(m)
^m = Matrix.matrixToList(t)
r = Matrix.matrixMult(t, 1.5)
IO.puts inspect m
IO.puts inspect Matrix.matrixToList(r)
IO.puts inspect Matrix.matrixToList(Matrix.matrixSum(t, r))

IO.puts "test vector"
l = [1.0, 2.0, 3.0, 4.0, 5.0, 9.0]
t = Matrix.listToVector(l)
^l = Matrix.vectorToList(t)
r = Matrix.matrixMult(t, 1.5)
IO.puts inspect l
IO.puts inspect Matrix.vectorToList(r)
IO.puts inspect Matrix.vectorToList(Matrix.matrixSum(t, r))

IO.puts "test mat * mat"
m = [[1.0, 2.0], [3.0, 4.0], [5.0, 9.0]]
l = [5.5, -1.5]
a = Matrix.listToMatrix(m)
b = Matrix.listToVector(l)
IO.puts inspect Matrix.matrixToList(Matrix.matrixMult(a, b))

m = [[1.0, 2.0, 3.0, 4.0], [5.0, 6.0, 7.0, 9.0]]
l = [[5.5, -1.5, 0.0], [0.5, 1.0, -1.0], [0.0, 2.0, 100.0], [100.0, 200.0, 400.0]]
l2 = [[5.5, -1.5, 0.0, 0.5], [1.0, -1.0, 0.0, 2.0], [100.0, 100.0, 200.0, 400.0]]
a = Matrix.listToMatrix(m)
b = Matrix.listToVector(l)
b2 = Matrix.listToVector(l2)
IO.puts inspect Matrix.matrixToList(Matrix.matrixMult(a, b))
IO.puts inspect Matrix.matrixToList(Matrix.matrixVectorMult(a, Matrix.transposeRowToColumn(Matrix.getRow(b2, 1))))
