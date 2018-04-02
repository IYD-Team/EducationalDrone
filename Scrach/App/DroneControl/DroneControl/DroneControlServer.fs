module DroneControlServer



// Scrach からデータを受信するためのHttpサーバー

let Server(port) = 
    let listener = new System.Net.HttpListener()
    listener.Prefixes.Add("http://127.0.0.1:"+(port |> string)+"/" )
    listener.Start()
    //System.Console.WriteLine("http://127.0.0.1:"+(port |> string)+"/" )


    

    while true do
    
        let context = listener.GetContext()
        let res = context.Response

        let mutable data = ""

        
        let path = context.Request.Url.PathAndQuery
        
        printfn "%s" path
        
        // リクエストの成功(OK 200)を返す
        res.StatusCode <- 200
        let sw = new System.IO.StreamWriter( res.OutputStream )
        sw.Write( data )
        sw.Close()

    ()