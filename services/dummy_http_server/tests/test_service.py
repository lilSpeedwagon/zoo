import http.client as http

def test_ping():
    # проверяем что сервис жив и отвечает на /ping
    
    address = 'localhost'
    port = 80
    connection = http.HTTPConnection(address, port)
    connection.connect()

    for i in range(10):
        connection.request('GET', '/ping', headers={'keep-alive': 'timeout=5'})
        response = connection.getresponse()
        body = response.read()
        print(f'{response.status} {str(body)}')

    connection.close()


test_ping()