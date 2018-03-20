defmodule Distributed do
    def connect(_, 0) do
        nil
    end
    def connect(nodeName, retries) do
        if Node.connect(nodeName) do
            true
        else
            connect(nodeName, retries - 1)
        end
    end
    def connect(nodeName) do
        connect(nodeName, 1000)
    end

    def connect_all([]) do
        nil
    end
    def connect_all([head | tail]) do
        if connect(head) do
            IO.puts("Connected to worker #{head}")
        else
            IO.puts("Can not connect to worker #{head}")
            IO.puts("List of all visible nodes: #{inspect Node.list([:visible, :hidden, :known])}")
            exit(:worker_unavailable)
        end
        connect_all(tail)
    end

    def map(workers, func, data) do
        pids = create_workers(workers)
        Enum.count(pids) |> split_data(data) |> Enum.zip(pids) |> send_tasks(func) |> inspect |> IO.puts
        recv_results(pids)
    end

    def map_worker(master) do
        IO.puts :stderr, "AAAAA"
        receive do
            {:do_map, ^master, func, data} ->
                send(master, {:map_result, Node.self(), do_map(func, data)})
        end
        IO.puts :stderr, "BBBBB"
        map_worker(master)
    end

    defp create_workers([worker | other]) do
        pid = Node.spawn_link(worker, Distributed, :map_worker, [Node.self()])
        IO.puts "ACDFQWR #{inspect pid}"
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
        send_tasks(other, func)
    end

    defp send_tasks([], _) do
        nil
    end

    defp do_map(func, [head | tail]) do
        IO.puts :stderr, "IN DO_MAP"
        [func.(head)] ++ do_map(func, tail)
    end
    defp do_map(_, []) do
        IO.puts :stderr, "END DO_MAP"
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
        IO.puts "I'm a Master"
        workers = String.split(workers, ",") |> Enum.map(fn x -> String.to_atom(x) end)
        Distributed.connect_all(workers)
        Distributed.map(workers, fn x -> x * x end, Enum.to_list(1..10))
        |> inspect
        |> IO.puts
    end

    def play([master: master]) do  # worker spec
        IO.puts :stderr, "I'm a Worker"
        if Distributed.connect(String.to_atom(master)) do
            IO.puts("Connected to master #{master}")
        end
        Distributed.map_worker(master)
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
