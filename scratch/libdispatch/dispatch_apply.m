#include <dispatch/dispatch.h>

int main(int argc, const char* argv[])
{
	dispatch_apply(8,dispatch_get_global_queue(0,0), ^(size_t i){
		for(long long k=0;k<100000000*1000L;k++);
	});
	return 0;
}
