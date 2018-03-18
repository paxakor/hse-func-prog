defmodule Vector do
  @on_load :init

  def init do
    :ok = :erlang.load_nif("../build/vector", 0)
  end

  def listToVector(_arg) do
    exit(:nif_not_loaded)
  end

  def vectorToList(_arg) do
    exit(:nif_not_loaded)
  end

  def vectorMult(_vector, _scalar) do
    exit(:nif_not_loaded)
  end

  def vectorSum(_lhs, _rhs) do
    exit(:nif_not_loaded)
  end
end

l = [1.0, 2.0, 3.0, 4.0, 5.0, 9.0]
t = Vector.listToVector(l)
^l = Vector.vectorToList(t)
IO.puts inspect l
r = Vector.vectorMult(t, 1.5)
IO.puts inspect Vector.vectorToList(r)
IO.puts inspect Vector.vectorToList(Vector.vectorSum(t, r))
