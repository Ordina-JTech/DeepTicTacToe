FROM jupyter/scipy-notebook

LABEL maintainer="Ordina Pythoneers <luuk.van.der.velden@ordina.nl>"

ENV PYTHONPATH /home/jovyan/work

# Install Tensorflow
RUN conda install --quiet --yes \
    'Flask=0.12*' \
    'tensorflow=1.3*' \
    'keras=2.0*' && \
    conda clean -tipsy && \
    fix-permissions $CONDA_DIR && \
    fix-permissions /home/$NB_USER
