defmodule Matrix do
  @on_load :init

  def init do
    :ok = :erlang.load_nif("../build/matrix", 0)
  end

  def listToMatrix(_arg) do
    exit(:nif_not_loaded)
  end

  def matrixToList(_arg) do
    exit(:nif_not_loaded)
  end

  def matrixMult(_matrix, _scalar) do
    exit(:nif_not_loaded)
  end

  def matrixSum(_lhs, _rhs) do
    exit(:nif_not_loaded)
  end
end

l = [[1.0, 2.0], [3.0, 4.0], [5.0, 9.0]]
t = Matrix.listToMatrix(l)
^l = Matrix.matrixToList(t)
IO.puts inspect l
r = Matrix.matrixMult(t, 1.5)
IO.puts inspect Matrix.matrixToList(r)
IO.puts inspect Matrix.matrixToList(Matrix.matrixSum(t, r))
