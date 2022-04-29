#include "aewb_logger_sender.h"
#include "aewb_logger_receiver.h"

#ifdef NDEBUG
#error "make sure cmake CMAKE_BUILD_TYPE is set to Debug, ow assert will be nop"
#endif

void test_send_receive_bytes() {
    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("192.168.5.3", 4321);
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("192.168.5.3", 4321);

    char *sbuf = (char*)&sender->buffer;
    sbuf[0] = 'a';
    sbuf[sizeof(sender->buffer)-1] = 'z';

    int32_t n_sent = aewb_logger_send_bytes(sender);
    assert(n_sent==sizeof(sender->buffer));

    int32_t n_recvd = aewb_logger_recv_bytes(receiver);
    assert(n_recvd==sizeof(sender->buffer));

    char *rbuf = (char*)&receiver->buffer;
    assert(rbuf[0]=='a');
    assert(rbuf[sizeof(receiver->buffer)-1]=='z');

    aewb_logger_destroy_sender(sender);
    aewb_logger_destroy_receiver(receiver);
}

void test_write_read_log()
{
    AewbHandle sender_handle;
    dcc_parser_input_params_t sender_dcc_input_params;
    awbprm_t sender_awb_params;
    tiae_prm_t sender_tiae_prm;

    sender_handle.ti_2a_wrapper.dcc_input_params = &sender_dcc_input_params;
    sender_handle.ti_2a_wrapper.p_ae_params = &sender_tiae_prm;  
    sender_handle.ti_2a_wrapper.p_awb_params = &sender_awb_params;

    sender_handle.aewb_config.awb_num_skip_frames = 121;
    sender_handle.ti_2a_wrapper.frame_count = 1313;
    sender_handle.ti_2a_wrapper.dcc_input_params->exposure_time = 1414;
    sender_handle.ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_range[0].min = 1515;
    sender_handle.sensor_in_data.ae_dynPrms.targetBrightness = 1616;
    sender_handle.sensor_out_data.aePrms.exposureTime[0] = 1717;
    
    log_AewbHandle buffer;

    int32_t n_written;
    n_written = aewb_logger_write_log_to_buffer(&sender_handle, &buffer);

    printf("test_write_read_log: n_written=%d\n",n_written);
    assert(n_written==sizeof(log_AewbHandle));

    assert(buffer.aewb_config.awb_num_skip_frames == 121);
    assert(buffer.ti_2a_wrapper.frame_count == 1313);
    assert(buffer.ti_2a_wrapper.dcc_input_params.exposure_time == 1414);
    assert(buffer.ti_2a_wrapper.ae_params.exposure_program.exposure_time_range[0].min == 1515);
    assert(buffer.ae_dynPrms.targetBrightness == 1616);
    assert(buffer.aePrms.exposureTime[0] == 1717);
}

void test_send_recv_log()
{
    AewbHandle sender_handle;
    dcc_parser_input_params_t sender_dcc_input_params;
    awbprm_t sender_awb_params;
    tiae_prm_t sender_tiae_prm;

    sender_handle.ti_2a_wrapper.dcc_input_params = &sender_dcc_input_params;
    sender_handle.ti_2a_wrapper.p_ae_params = &sender_tiae_prm;  
    sender_handle.ti_2a_wrapper.p_awb_params = &sender_awb_params;

    sender_handle.aewb_config.awb_num_skip_frames = 121;

    aewb_logger_sender_state_t *sender = aewb_logger_create_sender("192.168.5.3", 4321);
    aewb_logger_receiver_state_t *receiver = aewb_logger_create_receiver("192.168.5.3", 4321);

    int32_t n_written = aewb_logger_send_log(sender, &sender_handle);
    int32_t n_read = aewb_logger_recv_log(receiver);

    assert(n_written>1000);
    assert(n_written==n_read);
    assert(receiver->buffer.aewb_config.awb_num_skip_frames == 121);

    aewb_logger_destroy_sender(sender);
    aewb_logger_destroy_receiver(receiver);    
}

int main() 
{
    test_send_receive_bytes();
    test_write_read_log();
    test_send_recv_log();
}