library(readr);
library(tidyr);
library(dplyr);
library(magrittr);

csvToDataframe = function(path) {
    df <- read_csv(path, trim_ws=TRUE, progress=TRUE);
	
    #configurando para a extração automática dos dados
    extra_start = 9;
    extra_total = length(names(df)) - 8;
    extra_half = extra_total/2;

    # Fazer a subtração automaticamente
    for (i in c(extra_start:(extra_start+extra_half-1))){
        x = i;
        y = i+extra_half;
        df[,x] = df[,y] - df[,x];
    }

    # Remover as colunas extra que não são mais necessárias
    df <- df %>% select(1:(extra_start+extra_half-1));

    # Separar o nome do estado para obter
    # - Nome do arquivo
    # - Número da linha
    df <- df %>% rename(Thread = Container) %>% separate(Value, into=c("Value", "Location"), sep=" @", convert=TRUE) %>% separate(Location, into=c("File", "Line"), sep=":") %>% mutate(State = as.factor(State), Type = as.factor(Type), Value = as.factor(Value), Imbrication = as.integer(Imbrication), Line = as.integer(Line));

    # Criar uma identificação mais simples das threads
    dft <- df %>%  select(Thread) %>%  arrange(Thread) %>%  unique %>% mutate(ThreadId = 1:n())

    # Integrar a nova identificação no dataframe dos dados
    df <- df %>% left_join(dft, by="Thread") %>%  select(-Thread) %>% rename(Thread=ThreadId);

    # Criar uma coluna Origin que associa os dados ao nome do arquivo
    df <- df %>% mutate(Origin = as.factor("traces.csv"));

    rm(dft)

    return(df);
}

unionOfDataframes = function(df1_ipc,df2_misses){

	#eliminando o que não interessa para o arquivo de configuração
	df1_ipc <- df1_ipc %>%
	 select(Value,File,Line,PAPI_TOT_INS,PAPI_TOT_CYC,Thread) %>% as.data.frame();

	df2_misses <- df2_misses %>%
	 select(Value,File,Line,PAPI_L2_TCA,PAPI_L2_TCM,Thread) %>% as.data.frame();

	dfT <- df1_ipc %>% select(Value,File,Line,Thread) %>% as.data.frame() 

	dfT$totinst <- df1_ipc$PAPI_TOT_INS
	dfT$totcyc <- df1_ipc$PAPI_TOT_CYC
	dfT$tca <- df2_misses$PAPI_L2_TCA
	dfT$tcm <- df2_misses$PAPI_L2_TCM


    return(dfT);
}

df_ipcMetric <- csvToDataframe("../expData/cpu/traces.csv")

#df_missesMetric <- csvToDataframe("../../expData/mem/traces.csv")

#dfT <- unionOfDataframes(df_ipcMetric, df_missesMetric);

#dfT2 <- dfT %>% filter(grepl("omp",Value)) %>% as.data.frame();

#dfT3 <- dfT2 %>% filter(totinst > 0, totcyc > 0) %>% as.data.frame();

#dfT3$ipc <- dfT3$totinst / dfT3$totcyc
#dfT3$misses <- dfT3$tcm/dfT3$tca
#dfT3$maxfreq <- (dfT3$ipc<0.30 && dfT3$misses>0.35)

#dfT4 <- dfT3 %>% select(Line,maxfreq,Value,File) %>% as.data.frame();

#dfT5 <- dfT4[!duplicated(dfT4),]

#write.csv(dfT5, file = "../../expData/configurationFile.csv")
