// Learn more about F# at http://fsharp.org
// See the 'F# Tutorial' project for more help.


open DroneControlServer
open System

[<EntryPoint>]
let main argv = 
    let port = 
        if ( argv.Length = 0 ) then
            "COM6"
        else
            argv.[0]

    printfn "Start .NET DroneControl Server"

    flightController.Connect(port)
    flightController.Open()
    //let i = 0
    Server(5410)
    
    Console.ReadKey() |> ignore
    
    0