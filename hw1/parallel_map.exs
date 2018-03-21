defmodule Distributed do
    def connect(_, 0) do
        nil
    end
    def connect(nodeName, retries) when retries > 1 do
        if Node.connect(nodeName) do
            true
        else
            connect(nodeName, retries - 1)
        end
    end
    def connect(nodeName) do
        connect(nodeName, 1000)
    end

    def connect_all([], _) do
        nil
    end
    def connect_all([head | tail], node_name) do
        if connect(head) do
            IO.puts(:stderr, "Connected to #{node_name} #{head}")
        else
            IO.puts(:stderr, "Can not connect to #{node_name} #{head}")
            IO.puts(:stderr, "List of all visible nodes: #{inspect Node.list([:visible, :hidden, :known])}")
            exit(:worker_unavailable)
        end
        connect_all(tail, node_name)
    end

    def map(workers, func, data) do
        pids = create_workers(workers)
        Enum.count(pids) |> split_data(data) |> Enum.zip(pids) |> send_tasks(func)
        recv_results(workers)
    end

    def map_worker(master, acc) do
        receive do
            {:do_map, ^master, func, data} ->
                map_worker(master, acc ++ doMap(func, data))
            {:end_map, ^master, pid} ->
                send(pid, {:map_result, Node.self(), acc})
        end
    end

    defp create_workers([worker | other]) do
        pid = Node.spawn(worker, Distributed, :map_worker, [Node.self(), []])
        [pid] ++ create_workers(other)
    end

    defp create_workers([]) do
        []
    end

    defp recv_results([worker | other]) do
        receive do
            {:map_result, ^worker, result} ->
                result ++ recv_results(other)
        end
    end
    defp recv_results([]) do
        nil
    end

    defp split_data(count, data) when count > 1 do
        data_len = Enum.count(data)
        part_len = data_len |> div(count)
        part = Enum.slice(data, 0..(part_len - 1))
        other = Enum.slice(data, part_len..data_len)
        [part] ++ split_data(count - 1, other)
    end
    defp split_data(1, data) do
        [data]
    end

    defp send_tasks([worker_with_data | other], func) do
        {data, worker} = worker_with_data
        IO.puts "Sending task to worker: #{inspect worker}"
        send(worker, {:do_map, Node.self(), func, data})
        send(worker, {:end_map, Node.self(), self()})
        send_tasks(other, func)
    end
    defp send_tasks([], _) do
        nil
    end

    defp doMap(func, [head | tail]) do
        [func.(head)] ++ doMap(func, tail)
    end
    defp doMap(_, []) do
        []
    end


    def test() do
        data = Enum.to_list(1..14)
        result = split_data(4, data)
        if [[1, 2, 3], [4, 5, 6], [7, 8, 9, 10], [11, 12, 13, 14]] != result do
            IO.puts "error: #{inspect result} was not expected"
        end
    end
end

defmodule Test do
    def test_order() do
        range = Enum.to_list(1..10)
        f = fn x -> x * x end
        result = Distributed.map(f, range)
        expected = Enum.map(range, f)
        if result != expected do
            IO.puts("error: #{inspect(result)} != #{inspect(expected)}")
        end
    end

    def play([workers: workers]) do  # master spec
        workers = String.split(workers, ",") |> Enum.map(fn x -> String.to_atom(x) end)
        Distributed.connect_all(workers, "worker")
        :timer.sleep(1000)
        Distributed.map(workers, fn x -> x * x end, Enum.to_list(1..10))
        |> (fn res -> "Results: #{inspect res}" end).()
        |> IO.puts
    end

    def play([master: master]) do  # worker spec
        Distributed.connect_all([String.to_atom(master)], "master")
        # Distributed.map_worker(master, [])
    end

    def play([idle: true]) do
    end

    def main() do
        {args, _, _} = OptionParser.parse(System.argv())
        play(args)
    end
end

# Distributed.test()
Test.main()
