$port = 8000
$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add("http://localhost:$port/")
try {
    $listener.Start()
    Write-Host "AERO-SONAR Local Server running at http://localhost:$port/"
} catch {
    Write-Error "Failed to start listener on port $port : $_"
    exit 1
}

$folder = $PSScriptRoot
if (-not $folder) { $folder = (Get-Location).Path }

while ($listener.IsListening) {
    try {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response

        $relPath = $request.Url.LocalPath.TrimStart('/')
        if ([string]::IsNullOrEmpty($relPath) -or $relPath -eq '/') {
            $relPath = 'index.html'
        }
        $fullPath = Join-Path $folder $relPath

        if (Test-Path $fullPath -PathType Leaf) {
            $bytes = [System.IO.File]::ReadAllBytes($fullPath)
            if ($fullPath.EndsWith('.html')) {
                $response.ContentType = 'text/html; charset=utf-8'
            } elseif ($fullPath.EndsWith('.js')) {
                $response.ContentType = 'application/javascript'
            } elseif ($fullPath.EndsWith('.css')) {
                $response.ContentType = 'text/css'
            } elseif ($fullPath.EndsWith('.png')) {
                $response.ContentType = 'image/png'
            } else {
                $response.ContentType = 'application/octet-stream'
            }
            $response.ContentLength64 = $bytes.Length
            $response.OutputStream.Write($bytes, 0, $bytes.Length)
        } else {
            $response.StatusCode = 404
            $msg = [System.Text.Encoding]::UTF8.GetBytes("404 Not Found")
            $response.OutputStream.Write($msg, 0, $msg.Length)
        }
        $response.Close()
    } catch {
        # continue handling requests
    }
}
