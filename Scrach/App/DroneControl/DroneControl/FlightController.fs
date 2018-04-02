module FlightController





open System
open System.IO.Ports


type FlightController() =


    let mutable _socket:SerialPort = null
    let mutable _portName = "COM5"
    let mutable _baudRate = 57600

    
    /// Connect Serial on Arduino.
    member this.Connect( port:string ) =
        _socket <- new SerialPort( port, _baudRate )
        _socket.Open()
        printfn "Connected!"

        
    member this.Close() = 
        _socket.Close()
        _socket <- null

    member this.Open() =
        // Read data from arduino
        _socket.DataReceived.Add(fun (e) ->
            
            while _socket.BytesToRead > 0 do
                let head = _socket.ReadByte() |> byte

                printfn "%d" head


    )
 