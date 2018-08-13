#include "parser.h"

void atualizaDados(Data* data){
    ifstream arquivoAluno("instanciaNilbson.json", ios::in); // Abre instância com dados do aluno
    ifstream arquivoHorario("horario.txt", ios::in); // Abre instância com horários das disciplinas
    string horario1;
    
    json j; //Cria objeto da classe json para iterar os dados do aluno
    arquivoAluno >> j; // Transfere informações da instância para objeto do json

    int discUteis = -1;
    while (horario1 != "end"){ //Loop que identifica quantidade de disciplinas úteis
        arquivoHorario >> horario1; //Transfere informações dos horários para objeto do json
        if (horario1 == "0"){
            continue;
        }    
        ++discUteis;
    }
    arquivoHorario.close();

    //Fazendo segunda parte
    string *horario = new string[discUteis]; //Variável que armazena todos os horários
    ifstream arquivoHorario2("horario.txt", ios::in); // Abre instância com horários das disciplinas
    string horario2;
    
    int *identificador = new int[discUteis];
    int k = 0, temp = 0;
    while (horario2 != "end"){ //Loop que identifica quantidade de disciplinas úteis
        arquivoHorario2 >> horario2;
        
        if (horario2 != "0" && horario2 != "end"){
            identificador[k] = temp;
            horario[k] = horario2; //Armazena horarios p/ criação da matriz choque de horario
            k++;
        }
        temp++;
    }
    data->horario = horario;

    //OPTATIVAS - SEPARANDO AS OBRIGATÓRIAS E CONTANDO O NÚMERO DE OPTATIVAS
    int *identificadorOp = new int[discUteis];
    int *identificadorOb = new int[discUteis];
    int numOptativas = 0;
    int qntCreditoOpPago = 0;

    for(int i = 0; i < discUteis; i++)
        identificadorOb[i] = 0;
    for(int i = 0; i < discUteis; i++)
        identificadorOp[i] = 0;


    for (json::iterator it = j["tipoIntegralizacao"].begin(); it != j["tipoIntegralizacao"].end(); ++it){
        int i = atoi(it.key().data());
        
        //Organiza dados de acordo com os pre requisitos
        int retorno = 0, iTemp = 0;
        while (iTemp < discUteis){
            if (identificador[iTemp] == i){
                if (it.value() == "OP"){
                    identificadorOp[iTemp] = i;
                    numOptativas++;
                    break;
                }else{
                    identificadorOb[iTemp] = i;
                    break;
                }
            }
            iTemp++;
        }
    }
//------------------------------------------------------------------------------------------------------
    //Iterando json e armazenando Qnt. de créditos de cada cadeira
    int *credito = new int[discUteis];
    for (json::iterator it = j["cargaHoraria"].begin(); it != j["cargaHoraria"].end(); ++it) {
        int i = atoi(it.key().data());
        
        //Organiza dados de acordo com os que tem horário
        int retorno = 0, iTemp = 0;
        while (iTemp < discUteis){
            if (identificador[iTemp] == i){
                credito[iTemp] = it.value();
                credito[iTemp] /= 15;
            }
            iTemp++;
        }
    }

    //Iterando json e armazenando situacao (concluido = 1 ou pendente = 0)
    int *situacao = new int[discUteis];
    for (int i = 0; i < discUteis; i++){
        situacao[i] = 0;
    }

    for (json::iterator it = j["situacao"].begin(); it != j["situacao"].end(); ++it) {
        int i = atoi(it.key().data());
        
        //Organiza dados de acordo com os que tem horário
        int retorno = 0, iTemp = 0;
        while (iTemp < discUteis){
            if (identificador[iTemp] == i && (it.value() == "CONCLUIDO")){
                situacao[iTemp] = 1;

                if (identificadorOp[iTemp] == i){
                    qntCreditoOpPago += credito[iTemp];
                }
            }
            iTemp++;
        }
    }
    cout << "QUANTIDADE DE OPTATIVAS PAGAS: " << qntCreditoOpPago << endl;

    //ARMAZENA OS NOMES DAS CADEIRAS P/ USAR NO JSON DE SAÍDA DEPOIS DE RODAR O MODELO
    string *nomeTemp = new string[discUteis];
    for (json::iterator it = j["nome"].begin(); it != j["nome"].end(); ++it) {
        int i = atoi(it.key().data());
        
        //Organiza dados de acordo com os que tem horário
        int retorno = 0, iTemp = 0;
        while (iTemp < discUteis){
            if (identificador[iTemp] == i){
                nomeTemp[iTemp] = it.value();
            }
            iTemp++;
        }
    }

    //CRIA E PREENCHE MATRIZ DE CHOQUE DE HORÁRIOS
    bool choqueHorario[discUteis][discUteis]; //Armazena se tem choque de horario ou não
    for (int i = 0; i < discUteis; i++){
        for (int j = 0; j < discUteis; j++){
            if (horario[i] == horario[j])
                choqueHorario[i][j] = true;
            else
                choqueHorario[i][j] = false;
        }
    }

    //CRIA E PREENCHE MATRIZ DE PRE REQUISITOS
    int preReqI[discUteis][discUteis];//Matriz de pre requisitos
    for (int i = 0; i < discUteis; i++){
        for (int j = 0; j < discUteis; j++){
            preReqI[i][j] = 0;
        }
    }

    string preRequisito[discUteis][3]; //Lista de pre requsitos
    for (int i = 0; i < discUteis; i++){ //Zera essa lista
        for(int j = 0; j < 3; j++){
            preRequisito[i][j] = "0";
        }
    }

    for (json::iterator it = j["expressaoPreRequisito"].begin(); it != j["expressaoPreRequisito"].end(); ++it){
        int i = atoi(it.key().data());
        
        //Organiza dados de acordo com os pre requisitos
        int retorno = 0, iTemp = 0;
        while (iTemp < discUteis){
            if (identificador[iTemp] == i){
                retorno = 1;
                break;
            }
            iTemp++;
        }

        if (it.value() == nullptr)
            continue;
        //Passa para proxima iteração caso seja null
        /*if (it.value() == nullptr){ 
            preRequisito[iTemp] = "0";
            continue;
        }*/

        //Depois que encontrou pre requisito, armazena na string
        if (retorno == 1){
            preRequisito[iTemp][0] = it.value();
        }
    }
    
    //TRATA STRING PRE REQUISITO PARA ITERAR EM CODIGO
    for (int i = 0; i < discUteis; i++){
        if (preRequisito[i][0].size() == 1) //Se for null retorna 1
            continue;

        if (preRequisito[i][0].size() == 12){ //Apaga inicio e fim para filtrar código
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-10);
            preRequisito[i][0].erase(preRequisito[i][0].end()-3, preRequisito[i][0].end());
        }

        if (preRequisito[i][0].size() == 14){ //Apaga inicio e fim para filtrar código
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-12);
            preRequisito[i][0].erase(preRequisito[i][0].end()-3, preRequisito[i][0].end());
        }

        if (preRequisito[i][0].size() == 26){ //Apaga inicio e fim para filtrar código
            preRequisito[i][1] = preRequisito[i][0];
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-22);
            preRequisito[i][0].erase(preRequisito[i][0].end()-15, preRequisito[i][0].end());
            preRequisito[i][1].erase(preRequisito[i][1].begin(), preRequisito[i][1].end()-12);
            preRequisito[i][1].erase(preRequisito[i][1].end()-5, preRequisito[i][1].end());
        }

        if (preRequisito[i][0].size() == 28){ //Apaga inicio e fim para filtrar código
            preRequisito[i][1] = preRequisito[i][0];
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-24);
            preRequisito[i][0].erase(preRequisito[i][0].end()-15, preRequisito[i][0].end());
            if (preRequisito[i][0].at(8) == 'E'){
                preRequisito[i][0].erase(preRequisito[i][0].end()-2, preRequisito[i][0].end());
            }
            preRequisito[i][1].erase(preRequisito[i][1].begin(), preRequisito[i][1].end()-14);
            preRequisito[i][1].erase(preRequisito[i][1].end()-5, preRequisito[i][1].end());
            if (preRequisito[i][1].at(0) == 'E'){
                preRequisito[i][1].erase(preRequisito[i][1].begin(), preRequisito[i][1].end()-7);
            }
        }

        if (preRequisito[i][0].size() == 30){ //Apaga inicio e fim para filtrar código
            preRequisito[i][1] = preRequisito[i][0];
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-26);
            preRequisito[i][0].erase(preRequisito[i][0].end()-17, preRequisito[i][0].end());
            preRequisito[i][1].erase(preRequisito[i][1].begin(), preRequisito[i][1].end()-14);
            preRequisito[i][1].erase(preRequisito[i][1].end()-5, preRequisito[i][1].end());
        }

        if (preRequisito[i][0].size() == 36){ //Apaga inicio e fim para filtrar código
            preRequisito[i][1] = preRequisito[i][0];
            preRequisito[i][2] = preRequisito[i][0];
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-34);
            preRequisito[i][0].erase(preRequisito[i][0].end()-27, preRequisito[i][0].end());
            preRequisito[i][1].erase(preRequisito[i][1].begin(), preRequisito[i][1].end()-24);
            preRequisito[i][1].erase(preRequisito[i][1].end()-15, preRequisito[i][1].end());
            preRequisito[i][2].erase(preRequisito[i][2].begin(), preRequisito[i][2].end()-12);
            preRequisito[i][2].erase(preRequisito[i][2].end()-3, preRequisito[i][2].end());
        }

        if (preRequisito[i][0].size() == 42){ //Apaga inicio e fim para filtrar código
            preRequisito[i][1] = preRequisito[i][0];
            preRequisito[i][2] = preRequisito[i][0];
            preRequisito[i][0].erase(preRequisito[i][0].begin(), preRequisito[i][0].end()-38);
            preRequisito[i][0].erase(preRequisito[i][0].end()-29, preRequisito[i][0].end());
            preRequisito[i][1].erase(preRequisito[i][1].begin(), preRequisito[i][1].end()-26);
            preRequisito[i][1].erase(preRequisito[i][1].end()-17, preRequisito[i][1].end());
            preRequisito[i][2].erase(preRequisito[i][2].begin(), preRequisito[i][2].end()-14);
            preRequisito[i][2].erase(preRequisito[i][2].end()-5, preRequisito[i][2].end());
        }
    }

    //TRATA MATRIZ PRE REQUISITOS PARA TRANSFORMAR STRING EM IDENTIFICADOR
    int preReqIdentificador[discUteis][discUteis];
    for (int i = 0; i < discUteis; i++){ // Inicializa com requisito padrão
        for(int j = 0; j < discUteis; j++){
            preReqIdentificador[i][j] = 0;
        }
    }

    for (json::iterator it = j["codigo"].begin(); it != j["codigo"].end(); ++it){
        int i = atoi(it.key().data());
            
        //Organiza dados de acordo com os pre requisitos
        int iTemp = 0, posicao = 0;
    
        while (iTemp < discUteis){
            //cout << iTemp << "OI" << endl;
            for(int j = 0; j < 3; j++){
                //cout << preRequisito[iTemp][j] << "Ola galera" << endl;
                if(preRequisito[iTemp][j].compare(it.value()) == 0){
                    //cout << "Ola  " << " " << preRequisito[iTemp] << " " << it.value() << " " << iTemp << " " << i << endl;
                    //cout << preRequisito[iTemp][identificador[i]] << " " << it << endl;
                    for(k = 0; k < discUteis; k++){
                        if(identificador[k] == i){
                            posicao = k;
                        }
                    }
                    preReqIdentificador[iTemp][posicao] = i;
                    //cout << i << " - "<< preRequisito[iTemp][j] << " " << " " << it.value() << " "  << " j: "  << j << " " << preRequisito[iTemp][identificador[i]] << " " << i << " itemp: " << iTemp  << " identificador" << identificador[i] << endl;
                }
            }

            iTemp++;                                                                            
        }
    }
    

    /*for (int i = 0; i < discUteis; i++){
        cout << i << " : " << preRequisito[i] << endl;
    }*/

    //Atribuição dos dados que serão utilizados no modelo p/ a struct
    data->nomeCadeira = nomeTemp;
    data->creditos = credito;
    data->identificador = identificador;
    data->situacao = situacao;
    data->disciplinas = discUteis;
    data->numPeriodos = 12;
    data->numDisciplinasOp = numOptativas;
    data->numCreditosOp = 16 - qntCreditoOpPago; // De acordo com o sigaa p/ o curso de Engenharia de Computação
    data->identificadorOb = identificadorOb;
    data->identificadorOp = identificadorOp;

    //Preenche matriz de choque de horário direto em i
    data->choqueHorarioI = new bool*[data->disciplinas];
    for (int i = 0; i < data->disciplinas; i++){
        data->choqueHorarioI[i] = new bool[data->disciplinas];
        for (int j = 0; j < data->disciplinas; j++){
            data->choqueHorarioI[i][j] = choqueHorario[i][j];
        }
    }

    //Preenche matriz de pre-requisitos direto em i
    for (int i = 0; i < discUteis; i++){
        for (int j = 0; j < discUteis; j++){
            for(int k = 0; k < discUteis; k++){
                //cout << "i: " << i << " " << preReqIdentificador[i][j] << " ij " << identificador[k]<< endl;
                if (preReqIdentificador[i][j] == identificador[k]){
                    //cout << "*********************************************************************************************" << endl;
                    preReqI[i][j] = preReqIdentificador[i][j];
                    //cout << preReqI[i][j]  << " " << i << " " << j << "  Aquiiiiiiiiiiiiiiiiiiiiiiiiiiii" << endl;
                    break;
                }
                else{
                    //cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
                    preReqI[i][j] = 0;
                }
                //cout << endl << "pr" << preReqI[29][38] << endl;
            }
            //cout << endl << "se" << preReqI[29][38] << endl;
        }
        //cout << endl << "te" << preReqI[29][38] << endl;
    }

    data->preReqI = new int*[data->disciplinas];
    for (int i = 0; i < data->disciplinas; i++){
        data->preReqI[i] = new int[data->disciplinas];
        for (int j = 0; j < data->disciplinas; j++){
            data->preReqI[i][j] = preReqI[i][j];
        }
    }
    
    //Print genérico para visualizar se o parser exibe valores certos
    for (int i = 0; i < discUteis; i++){
        cout << i << " : " << identificador[i] << endl;
    }
    
    //Fecha arquivos
    arquivoAluno.close();
    arquivoHorario.close();
    arquivoHorario2.close();
}